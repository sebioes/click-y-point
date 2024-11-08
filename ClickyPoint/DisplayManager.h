#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <M5Unified.h>
#include <vector>
#include <string>

class DisplayManager {
public:
    DisplayManager();
    void begin();
    void clear();
    void printText(const char* text);
    void printIMUData(float gx, float gy, float gz);
    void drawCircle(int x, int y, int radius, uint16_t color);
    void drawRandomRect();
    void printMenuItems(const std::vector<std::string>& labels, size_t currentIndex);

private:
    void setTextProperties();
};

#endif 