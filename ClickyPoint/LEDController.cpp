#include "LEDController.h"

LEDController::LEDController()
    : currentBrightness(0), isBlinking(false),
      blinkInterval(500), lastBlinkTime(0), ledState(false) {
    M5.Power.setLed(currentBrightness);
}

void LEDController::setBrightness(uint8_t brightness) {
    if (brightness != currentBrightness) {
        M5.Power.setLed(brightness);
        currentBrightness = brightness;
        isBlinking = false; // Stop blinking when setting brightness externally
    }
}

void LEDController::blink(unsigned long interval) {
    isBlinking = true;
    blinkInterval = interval;
    lastBlinkTime = millis();
}

void LEDController::update() {
    if (isBlinking) {
        unsigned long currentTime = millis();
        if (currentTime - lastBlinkTime >= blinkInterval) {
            lastBlinkTime = currentTime;
            ledState = !ledState;
            
            uint8_t newBrightness = ledState ? 255 : 0;
            M5.Power.setLed(newBrightness);
            currentBrightness = newBrightness;
        }
    }
}

