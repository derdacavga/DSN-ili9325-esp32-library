#include "ILI9325Driver.h"

ILI9325Driver tft;

void setup() {
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawText(45, 10, "Dsn Was Here");
}

void loop() {
  for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 10; x++) {
      tft.drawPixel(x, y, TFT_RED);
    }
  }
}
