#include "DisplayManager.h"
#include <M5GFX.h>
#include <M5Unified.h>

DisplayManager::DisplayManager()
    : screenWidth(240), screenHeight(135), padding(10) {}

void DisplayManager::begin() {
  M5.Display.begin();
  setTextProperties();
}

void DisplayManager::setTextProperties() {
  M5.Display.setRotation(3); // Adjust rotation as needed
  M5.Display.setTextColor(GREEN);
  M5.Display.setTextDatum(textdatum_t::middle_center);
  M5.Display.setFont(&fonts::FreeSansBold9pt7b);
  M5.Display.setTextSize(1);
}

void DisplayManager::printText(const char *text, int x, int y,
                               const GFXfont *font, uint16_t color) {
  M5.Display.startWrite();
  M5.Display.setTextDatum(textdatum_t::top_left);
  M5.Display.setFont(font);
  M5.Display.setTextColor(color);
  M5.Display.setCursor(x, y);
  M5.Display.print(text);
  M5.Display.endWrite();
}

void DisplayManager::printIMUData(float gx, float gy, float gz) {
  M5.Display.clear(TFT_BLACK);
  M5.Display.printf("IMU:\r\n");
  M5.Display.printf("Gyro: %0.2f %0.2f %0.2f\r\n", gx, gy, gz);
}

void DisplayManager::printMenuItems(const std::vector<std::string> &labels,
                                    size_t currentIndex) {
  M5.Display.startWrite();
  M5.Display.fillScreen(TFT_BLACK);

  const int lineHeight = 20;
  const int startY = 20;
  const int textX = padding;

  for (size_t i = 0; i < labels.size(); i++) {
    uint16_t color = (i == currentIndex) ? GREEN : WHITE;
    M5.Display.setTextColor(color);
    M5.Display.setCursor(textX, startY + i * lineHeight);
    M5.Display.print(labels[i].c_str());
  }

  M5.Display.endWrite();
}

void DisplayManager::showDefaultScreen(const char *timeStr, const char *modeStr,
                                       const char *batteryStr, bool btStatus) {
  M5.Display.startWrite();

  M5.Display.clear(TFT_BLACK);
  M5.Display.setTextColor(WHITE);

  uint8_t batteryStrLength = strlen(batteryStr);

  M5.Display.setTextDatum(textdatum_t::top_left);
  M5.Display.setFont(&fonts::FreeSans12pt7b);
  M5.Display.setCursor(10, 15);
  M5.Display.print(timeStr);

  M5.Display.setFont(&fonts::FreeSans9pt7b);
  M5.Display.setCursor(10, 50);
  M5.Display.print(modeStr);

  // Battery Percentage
  M5.Display.setTextDatum(textdatum_t::top_right);
  M5.Display.setCursor(screenWidth - padding - batteryStrLength * 12, 15);
  M5.Display.print(batteryStr);

  // Battery Icon
  int batteryIconX =
      screenWidth - battery_png_width - 2 * padding - batteryStrLength * 12;
  int batteryIconY = 10;
  M5.Display.drawPng(battery_png, ~0u, batteryIconX, batteryIconY,
                     battery_png_width, battery_png_height, 0, 0, 1.0f, 1.0f,
                     datum_t::top_right);

  // Bluetooth Icon
  if (btStatus) {
    int btIconX = screenWidth - bluetooth_on_png_width - padding;
    int btIconY = 40;
    M5.Display.drawPng(bluetooth_on_png, ~0u, btIconX, btIconY,
                       bluetooth_on_png_width, bluetooth_on_png_height, 0, 0,
                       1.0f, 1.0f, datum_t::top_right);
  } else {
    int btIconX = screenWidth - bluetooth_off_png_width - padding;
    int btIconY = 40;
    M5.Display.drawPng(bluetooth_off_png, ~0u, btIconX, btIconY,
                       bluetooth_off_png_width, bluetooth_off_png_height, 0, 0,
                       1.0f, 1.0f, datum_t::top_right);
  }

  M5.Display.endWrite();
}
