#ifndef ROTARY_HANDLER_H
#define ROTARY_HANDLER_H

#include <Arduino.h>

class RotaryHandler {
public:
    RotaryHandler(uint8_t clkPin, uint8_t dtPin, uint8_t swPin);
    bool initialize();
    bool update();
    
    // Getters for current state
    int32_t getPosition() const;
    bool isButtonPressed() const;
    bool hasRotated() const;
    
private:
    // Pins
    const uint8_t clkPin;
    const uint8_t dtPin;
    const uint8_t swPin;
    
    // State tracking
    int32_t position = 0;
    bool buttonPressed = false;
    bool rotated = false;
    
    // Debouncing and state management
    uint8_t lastStateCLK;
    uint32_t lastButtonPress = 0;
    static const uint32_t DEBOUNCE_DELAY = 50; // ms
    
    // State tracking variables
    bool lastButtonState = true;  // true = not pressed (INPUT_PULLUP)
    bool buttonState = true;      // true = not pressed
};

#endif 