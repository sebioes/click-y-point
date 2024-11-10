#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <M5Unified.h>
#include <battery.h>
#include <battery_charging.h>
#include <bluetooth_on.h>
#include <bluetooth_off.h>
#include <string>
#include <vector>

class DisplayManager {
public:
    DisplayManager();
    void begin();
    void printText(const char* text, int x, int y, const GFXfont* font = &fonts::FreeSansBold9pt7b, uint16_t color = WHITE);
    void printIMUData(float gx, float gy, float gz);
    void printMenuItems(const std::vector<std::string>& labels, size_t currentIndex);
    void showSensitivityScreen(float sensitivity);
    void showDefaultScreen(const char* timeStr, const char* modeStr, const char* batteryStr, bool batteryCharging, bool btStatus);
    void resetPreviousValues();

private:
    const uint16_t screenWidth;
    const uint16_t screenHeight;
    const int padding;

    // Coordinates for ClipRects
    const int timeX = 10, timeY = 15, timeW = 100, timeH = 30;
    const int modeX = 10, modeY = 50, modeW = 140, modeH = 20;
    const int batteryX = 140, batteryY = 10, batteryW = 100, batteryH = 25;
    const int btX = 210, btY = 40, btW = 40, btH = 30;

    // Previous values for comparison
    std::string prevTimeStr;
    std::string prevModeStr;
    std::string prevBatteryStr;
    bool prevBatteryCharging;
    bool prevBtStatus;
};

#endif
