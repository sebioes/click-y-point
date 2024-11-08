#include "RotaryHandler.h"

RotaryHandler::RotaryHandler(uint8_t clkPin, uint8_t dtPin, uint8_t swPin)
    : clkPin(clkPin), dtPin(dtPin), swPin(swPin) {}

bool RotaryHandler::initialize() {
    // Setup pins
    pinMode(clkPin, INPUT);
    pinMode(dtPin, INPUT);
    pinMode(swPin, INPUT_PULLUP);
    
    // Get initial state
    lastStateCLK = digitalRead(clkPin);
    
    return true;
}

bool RotaryHandler::update() {
    rotated = false;    
    uint8_t currentStateCLK = digitalRead(clkPin);
    
    // Check for rotation
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
        // If DT state is different from CLK state, rotation is counter-clockwise
        if (digitalRead(dtPin) != currentStateCLK) {
            position--;
        } else {
            position++;
        }
        rotated = true;
    }
    
    lastStateCLK = currentStateCLK;
    
    bool currentButtonState = digitalRead(swPin);
    
    // Check if the button state changed
    if (currentButtonState != lastButtonState) {
        if (millis() - lastButtonPress > DEBOUNCE_DELAY) {
            buttonState = currentButtonState;
            // Button is pressed (LOW because of INPUT_PULLUP)
            if (buttonState == LOW) {
                buttonPressed = true;
            }
        }
        lastButtonPress = millis();
    } else {
        buttonPressed = false;
    }
    
    lastButtonState = currentButtonState;
    
    return true;
}

int32_t RotaryHandler::getPosition() const {
    return position;
}

bool RotaryHandler::isButtonPressed() const {
    return buttonPressed;
}

bool RotaryHandler::hasRotated() const {
    return rotated;
} 