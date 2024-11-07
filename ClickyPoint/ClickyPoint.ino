#include <M5Unified.h>

// Include all class headers
#include "IMUHandler.h"
#include "BLEHandler.h"
#include "LEDController.h"
#include "SoundManager.h"
#include "Application.h"

// Instantiate the Application
Application app;

void setup() {
    app.setup();
}

void loop() {
    app.loop();
}