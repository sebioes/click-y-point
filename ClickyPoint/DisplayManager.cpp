#include "DisplayManager.h"
#include "M5StickCPlus2.h"

DisplayManager::DisplayManager() {}

void DisplayManager::begin() {
    auto cfg = M5.config();
    StickCP2.begin(cfg);
    setTextProperties();
}

void DisplayManager::setTextProperties() {
    StickCP2.Display.setRotation(3);
    StickCP2.Display.setTextColor(GREEN);
    StickCP2.Display.setTextDatum(middle_center);
    StickCP2.Display.setFont(&fonts::FreeSansBold9pt7b);
    StickCP2.Display.setTextSize(1);
}

void DisplayManager::clear() {
    StickCP2.Display.clear();
}

void DisplayManager::printText(const char* text) {
    clear();
    StickCP2.Display.setCursor(10, 20);
    StickCP2.Display.printf("%s\r\n", text);
}

void DisplayManager::printIMUData(float gx, float gy, float gz) {
    clear();
    StickCP2.Display.printf("IMU:\r\n");
    StickCP2.Display.printf("Gyro: %0.2f %0.2f %0.2f\r\n", gx, gy, gz);
}

void DisplayManager::drawCircle(int x, int y, int radius, uint16_t color) {
    StickCP2.Display.fillCircle(x, y, radius, color);
}

void DisplayManager::drawRandomRect() {
    int x = rand() % StickCP2.Display.width();
    int y = rand() % StickCP2.Display.height();
    int r = (StickCP2.Display.width() >> 4) + 2;
    uint16_t c = rand();
    StickCP2.Display.fillRect(x - r, y - r, r * 2, r * 2, c);
} 