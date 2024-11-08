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

void DisplayManager::printMenuItems(const std::vector<std::string>& labels, size_t currentIndex) {
    clear();
    StickCP2.Display.setTextColor(WHITE);
    
    const int lineHeight = 20;
    const int startY = 20;
    
    for (size_t i = 0; i < labels.size(); i++) {
        if (i == currentIndex) {
            StickCP2.Display.setTextColor(GREEN);  // Highlight selected item
        } else {
            StickCP2.Display.setTextColor(WHITE);
        }
        
        StickCP2.Display.setCursor(10, startY + i * lineHeight);
        StickCP2.Display.printf("%s\r\n", labels[i].c_str());
    }
} 