#ifndef APPLICATION_H
#define APPLICATION_H

#include "BleDevice.h"
#include "SoundManager.h"
#include "LEDController.h"
#include "RotaryHandler.h"
#include "DisplayManager.h"
#include "Menu.h"

class Application {
public:
    Application();
    void setup();
    void loop();

private:
    // Methods
    void setupMenu();
    void togglePresentationMode();
    void resetCursor();
    void showIMUData();
    void updateMenu();
    void updateDefaultScreen();

    void handleIMUData();
    void handleConnectionEvents();
    void handleButtonPress();
    void updateSoundManager();
    void updateRotaryEncoderAndMenu();

    void adjustSensitivity();
    void updateSensitivityAdjustment();
    void startIMUDisplay();
    void updateIMUDisplay();
    void powerOffDevice();

    // Member variables
    BleDevice bleDevice;
    SoundManager soundManager;
    LEDController ledController;
    RotaryHandler rotaryHandler;
    DisplayManager displayManager;
    Menu menu;

    // Constants
    static constexpr uint16_t maxX = 1920; // Screen width
    static constexpr uint16_t maxY = 1080; // Screen height
    static constexpr uint16_t ledBlinkInterval = 500;       // LED blink interval
    static constexpr uint16_t leftClickDebounceDelay = 100;  // Left click debounce millis
    static constexpr uint16_t arrowCooldownDuration = 500;   // Arrow press cooldown millis
    static constexpr uint16_t arrowDetectionMaxIterations = 80; // Arrow threshold detection length
    static constexpr float leftInitialThreshold = -400.0f;
    static constexpr float leftFinalThreshold = 300.0f;
    static constexpr float rightInitialThreshold = 700.0f;
    static constexpr float rightFinalThreshold = 400.0f;
    static constexpr uint16_t menuTimeout = 5000;
    static constexpr uint16_t batteryLevelNumSamples = 100;

    // Pins
    static constexpr uint16_t buttonPin = 26;
    static constexpr uint16_t rotaryClkPin = 33;
    static constexpr uint16_t rotaryDtPin = 32;
    static constexpr uint16_t rotarySwPin = 0;

    // State variables
    bool presentationMode;
    float sensitivity;
    int32_t lastRotaryPosition;
    bool menuOpen;
    unsigned long menuLastInteractionTime;
    bool posToggle;
    bool wasConnected;
    unsigned long leftClickLastDebounceTime;
    bool leftClickPressed;
    unsigned long cooldownStartTime;
    bool inCooldown;
    bool leftInitialDetected;
    uint16_t leftIterationCount;
    bool rightInitialDetected;
    uint16_t rightIterationCount;
    unsigned long previousTime;
    int32_t batteryLevelSamples[batteryLevelNumSamples];
    uint16_t batteryLevelIndex;
    uint16_t batteryLevelCount;
    uint8_t previousAverageBatteryLevel;
    bool isCharging;
    bool inSensitivityAdjustment;
    bool inIMUDisplay;
};

#endif
