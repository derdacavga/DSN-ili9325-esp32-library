#include "ILI9325Driver.h"
#include "ILI9325Sprite.h"

#define MAX_BALLS 20

// --- Struct to hold the state of the balls ---
struct Ball {
  float x, y, dx, dy;
  int16_t old_x, old_y;  // Let's store the old position for each ball
};
Ball balls[MAX_BALLS];

// --- Global Objects ---
ILI9325Driver tft;
ILI9325Sprite ballTemplate(&tft);  // A SINGLE template for all balls
ILI9325Sprite fpsSprite(&tft);

// --- Settings and Constants ---
#define BALL_DIAMETER 20
const int16_t BALL_SIZE = BALL_DIAMETER + 1;
#define BG_COLOR TFT_NAVY
#define BALL_COLOR TFT_YELLOW
#define OBSTACLE_COLOR TFT_CYAN
#define TRANSPARENT_COLOR TFT_BLACK
const int TARGET_FPS = 60;
const int FRAME_TIME = 1000 / TARGET_FPS;

// --- Performance Tracking ---
unsigned long last_fps_time = 0;
int frame_count = 0;
int current_fps = 0;
int last_shown_fps = -1;

void setup() {
  tft.begin();

  // --- 1. DRAW THE STATIC BACKGROUND DIRECTLY TO THE SCREEN ---
  tft.fillScreen(BG_COLOR);
  tft.fillRect(80, 120, 80, 15, OBSTACLE_COLOR);
  tft.drawRect(80, 120, 80, 15, TFT_WHITE);

  // --- 2. PREPARE THE BALL TEMPLATE (ONLY ONCE) ---
  ballTemplate.createSprite(BALL_SIZE, BALL_SIZE);
  ballTemplate.fillSprite(TRANSPARENT_COLOR);
  const int16_t r = BALL_DIAMETER / 2;
  ballTemplate.fillCircle(r, r, r, BALL_COLOR);

  // --- 3. PREPARE THE FPS SPRITE ---
  fpsSprite.createSprite(90, 16);
  fpsSprite.setTextColor(TFT_GREEN, TRANSPARENT_COLOR);
  fpsSprite.setTextSize(2);

  // --- 4. INITIALIZE THE BALLS ---
  for (int i = 0; i < MAX_BALLS; i++) {
    balls[i].x = random(tft.width() - BALL_SIZE);
    balls[i].y = random(tft.height() - BALL_SIZE);
    balls[i].old_x = balls[i].x;  // At the start, old and new positions are the same
    balls[i].old_y = balls[i].y;
    do { balls[i].dx = (float)random(-30, 31) / 10.0; } while (abs(balls[i].dx) < 0.5);
    do { balls[i].dy = (float)random(-30, 31) / 10.0; } while (abs(balls[i].dy) < 0.5);
  }
  last_fps_time = millis();
}

void loop() {
  unsigned long frame_start_time = millis();

  // --- A. UNIFIED UPDATE AND DRAW LOOP ---
  // For each ball, perform the erase, update, draw, and save operations in sequence
  for (int i = 0; i < MAX_BALLS; i++) {

    // 1. ERASE THE PREVIOUS FRAME (SMART METHOD)
    // We are drawing over the old position of the ball with whatever should be behind it.
    // This solves the "square trail" issue.
    tft.fillCircle(balls[i].old_x + BALL_SIZE / 2, balls[i].old_y + BALL_SIZE / 2, BALL_SIZE / 2, BG_COLOR);
    if ((balls[i].old_x + BALL_SIZE > 80) && (balls[i].old_x < 160) && (balls[i].old_y + BALL_SIZE > 120) && (balls[i].old_y < 135)) {
      tft.fillRect(80, 120, 80, 15, OBSTACLE_COLOR);
      tft.drawRect(80, 120, 80, 15, TFT_WHITE);
    }

    // 2. UPDATE PHYSICS
    balls[i].x += balls[i].dx;
    balls[i].y += balls[i].dy;

    // Check for collisions...
    const int16_t r = BALL_DIAMETER / 2;
    if (balls[i].x < 0) {
      balls[i].x = 0;
      balls[i].dx = -balls[i].dx;
    }
    if (balls[i].x > tft.width() - BALL_SIZE) {
      balls[i].x = tft.width() - BALL_SIZE;
      balls[i].dx = -balls[i].dx;
    }
    if (balls[i].y < 0) {
      balls[i].y = 0;
      balls[i].dy = -balls[i].dy;
    }
    if (balls[i].y > tft.height() - BALL_SIZE) {
      balls[i].y = tft.height() - BALL_SIZE;
      balls[i].dy = -balls[i].dy;
    }
    if ((balls[i].x + BALL_SIZE > 80) && (balls[i].x < 160) && (balls[i].y + BALL_SIZE > 120) && (balls[i].y < 135)) {
      int16_t ball_center_x = (int16_t)balls[i].x + r;
      int16_t ball_center_y = (int16_t)balls[i].y + r;
      int16_t dx_center = ball_center_x - (80 + 40);
      int16_t dy_center = ball_center_y - (120 + 7);
      int16_t cross_x = (r + 40) - abs(dx_center);
      int16_t cross_y = (r + 7) - abs(dy_center);
      if (cross_x < cross_y) {
        balls[i].dx = -balls[i].dx;
        balls[i].x += (dx_center > 0 ? cross_x : -cross_x);
      } else {
        balls[i].dy = -balls[i].dy;
        balls[i].y += (dy_center > 0 ? cross_y : -cross_y);
      }
    }

    // 3. DRAW THE NEW FRAME
    ballTemplate.pushSprite((int16_t)balls[i].x, (int16_t)balls[i].y, TRANSPARENT_COLOR);

    // 4. SAVE THE POSITION FOR THE NEXT FRAME
    balls[i].old_x = balls[i].x;
    balls[i].old_y = balls[i].y;
  }

  // --- B. DRAW THE FPS UI ON TOP ---
  frame_count++;
  if (millis() - last_fps_time > 1000) {
    last_fps_time = millis();
    current_fps = frame_count;
    frame_count = 0;
  }
  if (current_fps != last_shown_fps) {
    // Erase the area behind the FPS text (only when the value changes)
    tft.fillRect(5, 5, 90, 16, BG_COLOR);
    last_shown_fps = current_fps;
    fpsSprite.fillSprite(TRANSPARENT_COLOR);
    fpsSprite.setCursor(0, 0);
    fpsSprite.print(current_fps);
    fpsSprite.print(" FPS");
  }
  // Draw the FPS sprite every frame to ensure it stays on top if a ball passes underneath
  fpsSprite.pushSprite(5, 5, TRANSPARENT_COLOR);

  // --- C. LIMIT THE FRAME RATE ---
  unsigned long frame_duration = millis() - frame_start_time;
  if (frame_duration < FRAME_TIME) {
    delay(FRAME_TIME - frame_duration);
  }
}