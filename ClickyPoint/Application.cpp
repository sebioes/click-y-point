#include "Application.h"
#include <cstdio>

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
      menuLastInteractionTime(0), batteryLevelIndex(0), batteryLevelCount(0),
      previousAverageBatteryLevel(0), isCharging(false) {}

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

  // Store in samples array
  batteryLevelSamples[batteryLevelIndex] = batteryLevel;
  batteryLevelIndex = (batteryLevelIndex + 1) % batteryLevelNumSamples;
  if (batteryLevelCount < batteryLevelNumSamples) {
    batteryLevelCount++;
  }

  // Compute average
  uint32_t sum = 0;
  for (int i = 0; i < batteryLevelCount; i++) {
    sum += batteryLevelSamples[i];
  }
  uint8_t averageBatteryLevel = sum / batteryLevelCount;

  char batteryString[16];
  if (averageBatteryLevel >= 0) {
    snprintf(batteryString, sizeof(batteryString), "%d%%", averageBatteryLevel);
  } else {
    snprintf(batteryString, sizeof(batteryString), "N/A");
  }

  // Get mode
  const char *modeString = presentationMode ? "Presentation" : "Normal";

  if (isCharging && averageBatteryLevel < previousAverageBatteryLevel) {
    isCharging = false;
  } else if (!isCharging && averageBatteryLevel > previousAverageBatteryLevel) {
    isCharging = true;
  }

  // Update display
  displayManager.showDefaultScreen(timeString, modeString, batteryString,
                                   isCharging, bleDevice.isConnected());

  previousAverageBatteryLevel = averageBatteryLevel;
}

void Application::setup() {
  // Initialize M5
  auto cfg = M5.config();
  cfg.serial_baudrate = 115200; // Set serial baud rate
  cfg.clear_display = true;     // Clear the screen on begin
  cfg.internal_imu = true;      // Use internal IMU
  cfg.internal_rtc = true;      // Use internal RTC
  cfg.internal_spk = true;      // Use internal speaker
  cfg.internal_mic = true;      // Use internal microphone
  cfg.led_brightness = 255;     // Set LED brightness (0-255)
  M5.begin(cfg);

  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize IMU
  auto imu_type = M5.Imu.getType();
  if (imu_type == m5::imu_none) {
    M5_LOGI("IMU not found, stopping.");
    while (true) {
      delay(1);
    }
  }
  M5.Imu.begin();
  M5.Power.begin();

  // Because power data is so jumpy, we take an moving average. We need to
  // initialize that average here
  for (int i = 0; i < batteryLevelNumSamples; i++) {
    batteryLevelSamples[i] = M5.Power.getBatteryLevel();
    M5.delay(1);
  }
  batteryLevelCount = batteryLevelNumSamples;

  soundManager.begin();

  bleDevice.begin(true, true);

  rotaryHandler.initialize();

  displayManager.begin();

  setupMenu();

  Serial.begin(115200);
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
      displayManager.resetPreviousValues();
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
  M5.Display.display();
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
          M5_LOGI("Cooldown ended");
        }
      }

      // If not in cooldown, proceed to detect gestures
      if (!inCooldown) {
        // --------- Left Arrow Detection ---------
        if (!leftInitialDetected) {
          if (val[1] <= leftInitialThreshold) {
            leftInitialDetected = true;
            leftIterationCount = 0;
            M5_LOGI("Left arrow: Initial threshold detected");
          }
        } else {
          leftIterationCount++;
          if (val[1] >= leftFinalThreshold) {
            // Trigger left arrow
            bleDevice.write(0xD8);
            M5_LOGI("Left arrow triggered");
            inCooldown = true;
            cooldownStartTime = currentTime;

            // Reset detection
            leftInitialDetected = false;
            leftIterationCount = 0;
          } else if (leftIterationCount >= arrowDetectionMaxIterations) {
            // Timeout
            leftInitialDetected = false;
            leftIterationCount = 0;
            M5_LOGI("Left arrow: Timeout");
          }
        }

        // --------- Right Arrow Detection ---------
        if (!rightInitialDetected) {
          if (val[1] >= rightInitialThreshold) {
            rightInitialDetected = true;
            rightIterationCount = 0;
            M5_LOGI("Right arrow: Initial threshold detected");
          }
        } else {
          rightIterationCount++;
          if (val[1] <= rightFinalThreshold) {
            // Trigger right arrow
            bleDevice.write(0xD7);
            M5_LOGI("Right arrow triggered");
            inCooldown = true;
            cooldownStartTime = currentTime;

            // Reset detection
            rightInitialDetected = false;
            rightIterationCount = 0;
          } else if (rightIterationCount >= arrowDetectionMaxIterations) {
            // Timeout
            rightInitialDetected = false;
            rightIterationCount = 0;
            M5_LOGI("Right arrow: Timeout");
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
      ledController.setBrightness(20);
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
