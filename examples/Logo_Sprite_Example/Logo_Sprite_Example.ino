#include "ILI9325Driver.h"
#include "ILI9325Sprite.h"
#include "dsn_logo.h"

#define MAX_BALLS 10

// --- Struct to hold the state of the balls ---
struct Ball {
  float x, y, dx, dy;
  int16_t old_x, old_y;
};
Ball balls[MAX_BALLS];

// --- Global Objects ---
ILI9325Driver tft;
ILI9325Sprite ballTemplate(&tft);
ILI9325Sprite fpsSprite(&tft);

// --- Settings and Constants ---
// MODIFICATION: The "ball" size is now derived directly from the image dimensions.
#define BALL_DIAMETER dsn_logo_width
const int16_t BALL_SIZE = dsn_logo_width;

#define BG_COLOR TFT_NAVY
#define OBSTACLE_COLOR TFT_CYAN
// MODIFICATION: White (0xFFFF) is set as the transparent color for the logo.
#define TRANSPARENT_COLOR 0xFFFF

const int TARGET_FPS = 60;
const int FRAME_TIME = 1000 / TARGET_FPS;

// --- Performance Tracking ---
unsigned long last_fps_time = 0;
int frame_count = 0;
int current_fps = 0;
int last_shown_fps = -1;

void setup() {
  tft.begin();

  tft.fillScreen(BG_COLOR);
  tft.fillRect(80, 120, 80, 15, OBSTACLE_COLOR);
  tft.drawRect(80, 120, 80, 15, TFT_WHITE);

  // --- MODIFICATION: The ballTemplate sprite is created and filled with your image data. ---
  ballTemplate.createSprite(dsn_logo_width, dsn_logo_height);
  ballTemplate.pushImage(0, 0, dsn_logo_width, dsn_logo_height, dsn_logo);

  fpsSprite.createSprite(90, 16);
  // MODIFICATION: Using BG_COLOR for the background makes the text look cleaner.
  fpsSprite.setTextColor(TFT_GREEN, BG_COLOR);
  fpsSprite.setTextSize(2);

  for (int i = 0; i < MAX_BALLS; i++) {
    balls[i].x = random(tft.width() - BALL_SIZE);
    balls[i].y = random(tft.height() - BALL_SIZE);
    balls[i].old_x = balls[i].x;
    balls[i].old_y = balls[i].y;
    do { balls[i].dx = (float)random(-30, 31) / 10.0; } while (abs(balls[i].dx) < 0.5);
    do { balls[i].dy = (float)random(-30, 31) / 10.0; } while (abs(balls[i].dy) < 0.5);
  }
  last_fps_time = millis();
}

void loop() {
  unsigned long frame_start_time = millis();

  for (int i = 0; i < MAX_BALLS; i++) {

    // --- MODIFICATION: Using fillRect to erase the old position. ---
    // This correctly clears the trail of the square-shaped sprite.
    tft.fillRect(balls[i].old_x, balls[i].old_y, BALL_SIZE, dsn_logo_height, BG_COLOR); // Use actual height
    if ((balls[i].old_x + BALL_SIZE > 80) && (balls[i].old_x < 160) && (balls[i].old_y + dsn_logo_height > 120) && (balls[i].old_y < 135)) {
      tft.fillRect(80, 120, 80, 15, OBSTACLE_COLOR);
      tft.drawRect(80, 120, 80, 15, TFT_WHITE);
    }

    balls[i].x += balls[i].dx;
    balls[i].y += balls[i].dy;

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
    if (balls[i].y > tft.height() - dsn_logo_height) { // Use actual height
      balls[i].y = tft.height() - dsn_logo_height;
      balls[i].dy = -balls[i].dy;
    }
    if ((balls[i].x + BALL_SIZE > 80) && (balls[i].x < 160) && (balls[i].y + dsn_logo_height > 120) && (balls[i].y < 135)) {
      int16_t ball_center_x = (int16_t)balls[i].x + r;
      int16_t ball_center_y = (int16_t)balls[i].y + (dsn_logo_height / 2);
      int16_t dx_center = ball_center_x - (80 + 40);
      int16_t dy_center = ball_center_y - (120 + 7);
      int16_t cross_x = (r + 40) - abs(dx_center);
      int16_t cross_y = ( (dsn_logo_height / 2) + 7) - abs(dy_center);
      if (cross_x < cross_y) {
        balls[i].dx = -balls[i].dx;
        balls[i].x += (dx_center > 0 ? cross_x : -cross_x);
      } else {
        balls[i].dy = -balls[i].dy;
        balls[i].y += (dy_center > 0 ? cross_y : -cross_y);
      }
    }

    // The ballTemplate sprite (now your image) is drawn to the screen.
    // Pixels with the TRANSPARENT_COLOR (white) will not be drawn.
    ballTemplate.pushSprite((int16_t)balls[i].x, (int16_t)balls[i].y, TRANSPARENT_COLOR);
    balls[i].old_x = balls[i].x;
    balls[i].old_y = balls[i].y;
  }

  frame_count++;
  if (millis() - last_fps_time > 1000) {
    last_fps_time = millis();
    current_fps = frame_count;
    frame_count = 0;
  }
  
  // *** LOGIC CORRECTION HERE ***
  // The sprite BUFFER is updated only when the FPS value changes (for efficiency).
  if (current_fps != last_shown_fps) {
    last_shown_fps = current_fps;
    fpsSprite.fillSprite(BG_COLOR); // Fill with opaque background color.
    fpsSprite.setCursor(0, 0);
    fpsSprite.print(current_fps);
    fpsSprite.print(" FPS");
  }
  
  // The sprite is PUSHED to the screen every frame to ensure it's always on top.
  fpsSprite.pushSprite(5, 5); // Push without transparency.

  unsigned long frame_duration = millis() - frame_start_time;
  if (frame_duration < FRAME_TIME) {
    delay(FRAME_TIME - frame_duration);
  }
}