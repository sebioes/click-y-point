#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <M5Unified.h>
#include <battery.h>
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
    void showDefaultScreen(const char* timeStr, const char* modeStr, const char* batteryStr, bool btStatus);

private:
    void setTextProperties();
    const uint16_t screenWidth;
    const uint16_t screenHeight;
    const int padding;
};

#endif
