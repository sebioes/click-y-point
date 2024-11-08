#ifndef APPLICATION_H
#define APPLICATION_H

#include "BLEHandler.h"
#include "IMUHandler.h"
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

    // Sound sequences
    static const ToneDuration connectMelody[];
    static const ToneDuration disconnectMelody[];
    static const ToneDuration resetCursorMelody[];
    static const SoundSequence connectSequence;
    static const SoundSequence disconnectSequence;
    static const SoundSequence resetCursorSequence;

    // Member variables
    BLEHandler bleHandler;
    IMUHandler imuHandler;
    SoundManager soundManager;
    LEDController ledController;
    RotaryHandler rotaryHandler;
    DisplayManager displayManager;
    Menu menu;

    bool posToggle;
    bool wasConnected;
    bool presentationMode;
    int lastRotaryPosition;

    // Constants
    static const uint16_t BLINK_INTERVAL = 500;
};

#endif
