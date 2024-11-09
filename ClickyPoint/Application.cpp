#include "Application.h"

// Define the melodies
const ToneDuration connectMelody[] = {
    {600, 120}, {0, 30}, {400, 120}, {0, 30}, {800, 120}};

const ToneDuration disconnectMelody[] = {{400, 150}, {0, 30}, {300, 150}};

const ToneDuration resetCursorMelody[] = {{100, 50}};

// Define the sound sequences
const SoundSequence connectSequence = {connectMelody, sizeof(connectMelody) /
                                                          sizeof(ToneDuration)};
const SoundSequence disconnectSequence = {
    disconnectMelody, sizeof(disconnectMelody) / sizeof(ToneDuration)};
const SoundSequence resetCursorSequence = {
    resetCursorMelody, sizeof(resetCursorMelody) / sizeof(ToneDuration)};

Application::Application()
    : bleDevice("Click y Point", "Click y Point"), presentationMode(false),
      sensitivity(35.0f), lastRotaryPosition(0),
      rotaryHandler(rotaryClkPin, rotaryDtPin, rotarySwPin), posToggle(true),
      wasConnected(true), leftClickLastDebounceTime(0), leftClickPressed(false),
      cooldownStartTime(0), inCooldown(false), leftInitialDetected(false),
      leftIterationCount(0), rightInitialDetected(false),
      rightIterationCount(0), previousTime(millis()), menuOpen(false),
      menuLastInteractionTime(0) {}

void Application::setupMenu() {
  // Add menu item with dynamic label for presentation mode
  menu.addItem(
      [this]() -> const char * {
        return presentationMode ? "Mode: Presentation" : "Mode: Normal";
      },
      [this]() { togglePresentationMode(); });

  // Add static menu items
  menu.addItem("Reset Cursor", [this]() { resetCursor(); });
  menu.addItem("Show IMU Data", [this]() { showIMUData(); });
}

void Application::togglePresentationMode() {
  presentationMode = !presentationMode;
  auto labels = menu.getAllLabels();
  displayManager.printMenuItems(labels, menu.getCurrentIndex());
}

void Application::resetCursor() {
  if (bleDevice.isConnected()) {
    soundManager.playSoundSequence(&resetCursorSequence);
    if (posToggle) {
      bleDevice.setPosition(960, 540, maxX, maxY);
    } else {
      bleDevice.setPosition(961, 541, maxX, maxY);
    }
    posToggle = !posToggle;
  }
}

void Application::showIMUData() {
  if (M5.Imu.update()) {
    float val[3];
    M5.Imu.getGyro(&val[0], &val[1], &val[2]);
    displayManager.printIMUData(val[0], val[1], val[2]);
  }
}

void Application::updateMenu() {
  if (rotaryHandler.hasRotated()) {
    if (rotaryHandler.getPosition() > lastRotaryPosition) {
      menu.next();
    } else {
      menu.previous();
    }
    lastRotaryPosition = rotaryHandler.getPosition();

    // Update display with all menu items
    auto labels = menu.getAllLabels();
    displayManager.printMenuItems(labels, menu.getCurrentIndex());

    // Reset the menu timeout
    menuLastInteractionTime = millis();
  }

  if (rotaryHandler.isButtonPressed()) {
    menu.select();
    // Reset the menu timeout
    menuLastInteractionTime = millis();
  }
}

void Application::updateDefaultScreen() {
  // Get the time
  m5::rtc_datetime_t dt;
  char timeString[16] = "Time N/A";
  if (M5.Rtc.isEnabled() && M5.Rtc.getDateTime(&dt)) {
    snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", dt.time.hours,
             dt.time.minutes, dt.time.seconds);
  }

  // Get battery level
  int batteryLevel = M5.Power.getBatteryLevel();
  char batteryString[16];
  if (batteryLevel >= 0) {
    snprintf(batteryString, sizeof(batteryString), "Battery: %d%%",
             batteryLevel);
  } else {
    snprintf(batteryString, sizeof(batteryString), "Battery N/A");
  }

  // Get mode
  const char *modeString =
      presentationMode ? "Mode: Presentation" : "Mode: Normal";

  // Update display
  displayManager.showDefaultScreen(timeString, batteryString, modeString);
}

void Application::setup() {
  // Initialize M5
  auto cfg = M5.config();
  cfg.led_brightness = 255;
  M5.begin(cfg);

  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize Serial
  Serial.begin(115200);

  // Initialize IMU
  auto imu_type = M5.Imu.getType();
  if (imu_type == m5::imu_none) {
    Serial.println("IMU not found, stopping.");
    while (true) {
      delay(1);
    }
  }
  M5.Imu.init();
  Serial.printf("IMU initialized.\n");

  soundManager.begin();

  bleDevice.begin(true, true);

  rotaryHandler.initialize();

  displayManager.begin();

  setupMenu();

  auto labels = menu.getAllLabels();
  displayManager.printMenuItems(labels, menu.getCurrentIndex());
}

void Application::loop() {
  M5.update();
  handleConnectionEvents();
  ledController.update();
  soundManager.update();
  rotaryHandler.update();

  if (menuOpen) {
    updateMenu();

    // Close menu after timeout
    if (millis() - menuLastInteractionTime > menuTimeout) {
      menuOpen = false;
      // Show default screen when menu closes
      updateDefaultScreen();
    }
  } else {
    handleIMUData();
    handleButtonPress();
    // Update default screen periodically
    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime > 1000) { // Update every second
      lastUpdateTime = millis();
      updateDefaultScreen();
    }

    // Check if rotary button is pressed to open menu
    if (rotaryHandler.isButtonPressed()) {
      menuOpen = true;
      menuLastInteractionTime = millis();

      // Display the menu
      auto labels = menu.getAllLabels();
      displayManager.printMenuItems(labels, menu.getCurrentIndex());
    }
  }
}

// Refactored method to handle IMU data
void Application::handleIMUData() {
  static unsigned long previousTime = millis();
  unsigned long currentTime = millis();

  if (M5.Imu.update() && bleDevice.isConnected()) {
    float val[3];
    M5.Imu.getGyro(&val[0], &val[1], &val[2]);

    if (presentationMode) {
      // --------- Cooldown Management ---------
      if (inCooldown) {
        if (currentTime - cooldownStartTime > arrowCooldownDuration) {
          inCooldown = false;
          Serial.println("Cooldown ended");
        }
      }

      // If not in cooldown, proceed to detect gestures
      if (!inCooldown) {
        // --------- Left Arrow Detection ---------
        if (!leftInitialDetected) {
          if (val[1] <= leftInitialThreshold) {
            leftInitialDetected = true;
            leftIterationCount = 0;
            Serial.println("Left arrow: Initial threshold detected");
          }
        } else {
          leftIterationCount++;
          if (val[1] >= leftFinalThreshold) {
            // Trigger left arrow
            bleDevice.write(0xD8);
            Serial.println("Left arrow triggered");
            inCooldown = true;
            cooldownStartTime = currentTime;

            // Reset detection
            leftInitialDetected = false;
            leftIterationCount = 0;
          } else if (leftIterationCount >= arrowDetectionMaxIterations) {
            // Timeout
            leftInitialDetected = false;
            leftIterationCount = 0;
            Serial.println("Left arrow: Timeout");
          }
        }

        // --------- Right Arrow Detection ---------
        if (!rightInitialDetected) {
          if (val[1] >= rightInitialThreshold) {
            rightInitialDetected = true;
            rightIterationCount = 0;
            Serial.println("Right arrow: Initial threshold detected");
          }
        } else {
          rightIterationCount++;
          if (val[1] <= rightFinalThreshold) {
            // Trigger right arrow
            bleDevice.write(0xD7);
            Serial.println("Right arrow triggered");
            inCooldown = true;
            cooldownStartTime = currentTime;

            // Reset detection
            rightInitialDetected = false;
            rightIterationCount = 0;
          } else if (rightIterationCount >= arrowDetectionMaxIterations) {
            // Timeout
            rightInitialDetected = false;
            rightIterationCount = 0;
            Serial.println("Right arrow: Timeout");
          }
        }
      }
    } else {
      unsigned long deltaMs = currentTime - previousTime;
      previousTime = currentTime;

      if (deltaMs == 0)
        deltaMs = 10; // Assume 10ms

      int8_t movement_x =
          static_cast<int8_t>(-val[2] * deltaMs * sensitivity / 1000.0f);
      int8_t movement_y =
          static_cast<int8_t>(-val[0] * deltaMs * sensitivity / 1000.0f);

      bleDevice.move(movement_x, movement_y);
    }
  }
}

// Refactored method to handle connection events
void Application::handleConnectionEvents() {
  if (bleDevice.isConnected()) {
    if (!wasConnected) {
      ledController.setBrightness(80);
      soundManager.playSoundSequence(&connectSequence);
      wasConnected = true;
    }
  } else {
    if (wasConnected) {
      ledController.blink(ledBlinkInterval);
      soundManager.playSoundSequence(&disconnectSequence);
      wasConnected = false;
    }
  }
}

// Refactored method to handle button press
void Application::handleButtonPress() {
  unsigned long currentTime = millis();

  if (M5.BtnB.wasPressed()) {
    resetCursor();
  }

  if (digitalRead(buttonPin) == LOW) {
    if (!leftClickPressed &&
        (currentTime - leftClickLastDebounceTime) > leftClickDebounceDelay) {
      bleDevice.press(1);
      leftClickPressed = true;
      leftClickLastDebounceTime = currentTime;
    }
  } else {
    if (leftClickPressed &&
        (currentTime - leftClickLastDebounceTime) > leftClickDebounceDelay) {
      bleDevice.release(1);
      leftClickPressed = false;
      leftClickLastDebounceTime = currentTime;
    }
  }
}
