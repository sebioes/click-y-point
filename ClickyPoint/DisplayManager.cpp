#include "DisplayManager.h"
#include <M5Unified.h>

DisplayManager::DisplayManager()
    : screenWidth(240), screenHeight(135), padding(10), prevTimeStr(""),
      prevModeStr(""), prevBatteryStr(""), prevBatteryCharging(false),
      prevBtStatus(false) {}

void DisplayManager::begin() {
  M5.Display.begin();
  M5.Display.setEpdMode(epd_mode_t::epd_fastest);
  M5.Display.setRotation(1);
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

void DisplayManager::printMenuItems(const std::vector<std::string> &labels,
                                    size_t currentIndex) {
  M5.Display.startWrite();
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextDatum(textdatum_t::middle_left);
  M5.Display.setFont(&fonts::FreeSans12pt7b);

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

void DisplayManager::showSensitivityScreen(float sensitivity) {
  M5.Display.startWrite();
  M5.Display.fillScreen(TFT_BLACK);

  // Display the sensitivity value
  char sensitivityStr[16];
  snprintf(sensitivityStr, sizeof(sensitivityStr), "Sensitivity: %.1f",
           sensitivity);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextDatum(textdatum_t::middle_center);
  M5.Display.setFont(&fonts::FreeSans12pt7b);
  M5.Display.drawString(sensitivityStr, screenWidth / 2, screenHeight / 4);

  // Display the sensitivity bar
  int barX = 20;
  int barY = screenHeight / 2;
  int barWidth = screenWidth - 40;
  int barHeight = 20;
  M5.Display.drawRect(barX, barY, barWidth, barHeight, WHITE);

  // Fill the bar based on sensitivity value
  float normalizedValue = (sensitivity - 10.0f) / 40.0f; // Range is 10 to 50
  int filledWidth = static_cast<int>(barWidth * normalizedValue);
  M5.Display.fillRect(barX + 1, barY + 1, filledWidth - 2, barHeight - 2,
                      GREEN);

  M5.Display.endWrite();
}

void DisplayManager::printIMUData(float gx, float gy, float gz) {
  M5.Display.startWrite();
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextDatum(textdatum_t::top_left);
  M5.Display.setFont(&fonts::FreeSans12pt7b);
  M5.Display.setCursor(10, 20);
  M5.Display.printf("IMU Data:\n");
  M5.Display.printf("Gyro X: %0.2f\n", gx);
  M5.Display.printf("Gyro Y: %0.2f\n", gy);
  M5.Display.printf("Gyro Z: %0.2f\n", gz);
  M5.Display.endWrite();
}

void DisplayManager::showDefaultScreen(const char *timeStr, const char *modeStr,
                                       const char *batteryStr,
                                       bool batteryCharging, bool btStatus) {
  M5.Display.startWrite();
  M5.Display.waitDisplay();

  // Time
  if (prevTimeStr != timeStr) {
    M5.Display.setClipRect(timeX, timeY, timeW, timeH);
    M5.Display.fillRect(timeX, timeY, timeW, timeH,
                        TFT_BLACK); // Clear the area
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextDatum(textdatum_t::top_left);
    M5.Display.setFont(&fonts::FreeSans12pt7b);
    M5.Display.setCursor(timeX, timeY);
    M5.Display.print(timeStr);
    M5.Display.clearClipRect();
    prevTimeStr = timeStr;
  }

  // Mode
  if (prevModeStr != modeStr) {
    M5.Display.setClipRect(modeX, modeY, modeW, modeH);
    M5.Display.fillRect(modeX, modeY, modeW, modeH,
                        TFT_BLACK); // Clear the area
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextDatum(textdatum_t::top_left);
    M5.Display.setFont(&fonts::FreeSans9pt7b);
    M5.Display.setCursor(modeX, modeY);
    M5.Display.print(modeStr);
    M5.Display.clearClipRect();
    prevModeStr = modeStr;
  }

  // Battery
  if (prevBatteryStr != batteryStr || prevBatteryCharging != batteryCharging) {
    M5.Display.setClipRect(batteryX, batteryY, batteryW, batteryH);
    M5.Display.fillRect(batteryX, batteryY, batteryW, batteryH,
                        TFT_BLACK); // Clear the area
    uint8_t batteryStrLength = strlen(batteryStr);
    M5.Display.setTextDatum(textdatum_t::top_right);
    M5.Display.setFont(&fonts::FreeSans9pt7b);
    M5.Display.setCursor(screenWidth - padding - batteryStrLength * 12,
                         batteryY + 7);
    M5.Display.print(batteryStr);

    // Battery Icon
    if (batteryCharging) {
      int batteryIconX = screenWidth - battery_charging_png_width -
                         2 * padding - batteryStrLength * 12;
      int batteryIconY = batteryY;
      M5.Display.drawPng(battery_charging_png, ~0u, batteryIconX, batteryIconY,
                         battery_charging_png_width,
                         battery_charging_png_height, 0, 0, 1.0f, 1.0f,
                         datum_t::top_right);
    } else {
      int batteryIconX =
          screenWidth - battery_png_width - 2 * padding - batteryStrLength * 12;
      int batteryIconY = batteryY;
      M5.Display.drawPng(battery_png, ~0u, batteryIconX, batteryIconY,
                         battery_png_width, battery_png_height, 0, 0, 1.0f,
                         1.0f, datum_t::top_right);
    }
    M5.Display.clearClipRect();
    prevBatteryStr = batteryStr;
    prevBatteryCharging = batteryCharging;
  }

  // Bluetooth
  if (prevBtStatus != btStatus) {
    M5.Display.setClipRect(btX, btY, btW, btH);
    M5.Display.fillRect(btX, btY, btW, btH, TFT_BLACK); // Clear the area
    int btIconX = btX - padding;
    int btIconY = btY;
    if (btStatus) {
      M5.Display.drawPng(bluetooth_on_png, ~0u, btIconX, btIconY,
                         bluetooth_on_png_width, bluetooth_on_png_height, 0, 0,
                         1.0f, 1.0f, datum_t::top_right);
    } else {
      M5.Display.drawPng(bluetooth_off_png, ~0u, btIconX, btIconY,
                         bluetooth_off_png_width, bluetooth_off_png_height, 0,
                         0, 1.0f, 1.0f, datum_t::top_right);
    }
    M5.Display.clearClipRect();
    prevBtStatus = btStatus;
  }

  M5.Display.endWrite();
}

void DisplayManager::resetPreviousValues() {
  M5.Display.clear(TFT_BLACK);
  prevTimeStr = "";
  prevModeStr = "";
  prevBatteryStr = "";
  prevBtStatus = !prevBtStatus; // Force update
}
