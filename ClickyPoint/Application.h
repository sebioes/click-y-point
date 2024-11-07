#ifndef APPLICATION_H
#define APPLICATION_H

#include "IMUHandler.h"
#include "BLEHandler.h"
#include "LEDController.h"
#include "SoundManager.h"

struct SoundSequence; // Forward declaration

class Application {
public:
    Application();
    void setup();
    void loop();

private:
    IMUHandler imuHandler;
    BLEHandler bleHandler;
    LEDController ledController;
    SoundManager soundManager;

    // Position Toggle
    bool posToggle;

    // LED Blinking Parameters
    const unsigned long BLINK_INTERVAL = 500; // milliseconds
    bool wasConnected;

    bool presentationMode;

    // Define Melodies
    static const ToneDuration connectMelody[];
    static const ToneDuration disconnectMelody[];
    static const ToneDuration resetCursorMelody[];
    static const SoundSequence connectSequence;
    static const SoundSequence disconnectSequence;
    static const SoundSequence resetCursorSequence;

    // Handle Connection Events
    void handleConnectionEvents();
    // Handle Button Press
    void handleButtonPress();
};

#endif
