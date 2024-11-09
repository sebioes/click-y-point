#include "DisplayManager.h"
#include <M5Unified.h>

DisplayManager::DisplayManager() {}

void DisplayManager::begin() {
    setTextProperties();
}

void DisplayManager::setTextProperties() {
    M5.Display.setRotation(3);
    M5.Display.setTextColor(GREEN);
    M5.Display.setTextDatum(middle_center);
    M5.Display.setFont(&fonts::FreeSansBold9pt7b);
    M5.Display.setTextSize(1);
}

void DisplayManager::clear() {
    M5.Display.clear();
}

void DisplayManager::printText(const char* text) {
    clear();
    M5.Display.setCursor(10, 20);
    M5.Display.printf("%s\r\n", text);
}

void DisplayManager::printIMUData(float gx, float gy, float gz) {
    clear();
    M5.Display.printf("IMU:\r\n");
    M5.Display.printf("Gyro: %0.2f %0.2f %0.2f\r\n", gx, gy, gz);
}

void DisplayManager::printMenuItems(const std::vector<std::string>& labels, size_t currentIndex) {
    clear();
    M5.Display.setTextColor(WHITE);
    
    const int lineHeight = 20;
    const int startY = 20;
    
    for (size_t i = 0; i < labels.size(); i++) {
        if (i == currentIndex) {
            M5.Display.setTextColor(GREEN);  // Highlight selected item
        } else {
            M5.Display.setTextColor(WHITE);
        }
        
        M5.Display.setCursor(10, startY + i * lineHeight);
        M5.Display.printf("%s\r\n", labels[i].c_str());
    }
} 

void DisplayManager::showDefaultScreen(const char* timeStr, const char* batteryStr, const char* modeStr) {
    clear();
    M5.Display.setTextColor(TFT_WHITE);

    // Center the text on the display
    int centerX = M5.Display.width() / 2;
    int startY = 20;
    int lineHeight = 30;

    M5.Display.setTextDatum(textdatum_t::top_center);

    M5.Display.drawString(timeStr, centerX, startY);
    M5.Display.drawString(batteryStr, centerX, startY + lineHeight);
    M5.Display.drawString(modeStr, centerX, startY + 2 * lineHeight);
}
