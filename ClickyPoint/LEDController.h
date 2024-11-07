#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <M5Unified.h>

class LEDController {
public:
    LEDController();
    void setBrightness(uint8_t brightness);
    void blink(unsigned long interval = 500);
    void update();

private:
    uint8_t currentBrightness;
    bool isBlinking;
    unsigned long blinkInterval;
    unsigned long lastBlinkTime;
    bool ledState;
};

#endif