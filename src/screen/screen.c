#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include "main.c"

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

bool isTouched(int x, int y, int xMin, int xMax, int yMin, int yMax) {
  return (x >= xMin && x <= xMax && y >= yMin && y <= yMax);
}

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  
  tft.fillScreen(ILI9341_BLACK);
  tft.drawBitmap(0, 0, "/assets/MainMenuV1.png", "253", "280", ILI9341_WHITE);

  int touchX = 0
  int touchY = 0
  
  if (isTouched(touchX, touchY, xMin, xMax, yMin, yMax))
  {
    Serial.println("touched example");
  }
}
