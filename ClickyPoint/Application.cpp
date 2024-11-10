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
      rightIterationCount(0), previousTime(millis()), batteryLevelIndex(0),
      batteryLevelCount(0), previousAverageBatteryLevel(0), isCharging(false),
      currentState(AppState::Default) {}

void Application::setupMenu() {
  menu.addItem(
      [this]() -> std::string {
        return presentationMode ? "Mode: Presentation" : "Mode: Normal";
      },
      [this]() { togglePresentationMode(); });

  menu.addItem("Change Sensitivity", [this]() { adjustSensitivity(); });

  menu.addItem("Show IMU Data", [this]() { startIMUDisplay(); });

  menu.addItem("Power Off", [this]() { powerOffDevice(); });
}

void Application::togglePresentationMode() {
  presentationMode = !presentationMode;
  if (currentState == AppState::Menu) {
    auto labels = menu.getAllLabels();
    displayManager.printMenuItems(labels, menu.getCurrentIndex());
  }
}

void Application::adjustSensitivity() {
  currentState = AppState::SensitivityAdjustment;
  lastRotaryPosition = rotaryHandler.getPosition();
  displayManager.showSensitivityScreen(sensitivity);
}

void Application::updateSensitivityAdjustment() {
  if (rotaryHandler.hasRotated()) {
    int32_t delta = lastRotaryPosition - rotaryHandler.getPosition();
    lastRotaryPosition = rotaryHandler.getPosition();
    sensitivity += delta;
    sensitivity = std::clamp(sensitivity, 10.0f, 50.0f);
    displayManager.showSensitivityScreen(sensitivity);
  }

  if (rotaryHandler.isButtonPressed()) {
    currentState = AppState::Menu;
    menuLastInteractionTime = millis();
    auto labels = menu.getAllLabels();
    displayManager.printMenuItems(labels, menu.getCurrentIndex());
  }
}

void Application::startIMUDisplay() {
  currentState = AppState::IMUDisplay;
  displayManager.printIMUData(0.0f, 0.0f, 0.0f);
}

void Application::updateIMUDisplay() {
  if (M5.Imu.update()) {
    float val[3];
    M5.Imu.getGyro(&val[0], &val[1], &val[2]);
    displayManager.printIMUData(val[0], val[1], val[2]);
  }

  if (rotaryHandler.isButtonPressed()) {
    currentState = AppState::Menu;
    menuLastInteractionTime = millis();
    auto labels = menu.getAllLabels();
    displayManager.printMenuItems(labels, menu.getCurrentIndex());
  }
}

void Application::powerOffDevice() { M5.Power.powerOff(); }

void Application::resetCursor() {
  if (bleDevice.isConnected()) {
    soundManager.playSoundSequence(&resetCursorSequence);
    bleDevice.setPosition(posToggle ? 960 : 961, posToggle ? 540 : 541, maxX,
                          maxY);
    posToggle = !posToggle;
  }
}

void Application::updateMenu() {
  if (rotaryHandler.hasRotated()) {
    if (rotaryHandler.getPosition() < lastRotaryPosition) {
      menu.next();
    } else {
      menu.previous();
    }
    lastRotaryPosition = rotaryHandler.getPosition();

    auto labels = menu.getAllLabels();
    displayManager.printMenuItems(labels, menu.getCurrentIndex());

    menuLastInteractionTime = millis();
  }

  if (rotaryHandler.isButtonPressed()) {
    menu.select();
    menuLastInteractionTime = millis();
  }
}

void Application::updateDefaultScreen() {
  m5::rtc_datetime_t dt;
  char timeString[16] = "Time N/A";
  if (M5.Rtc.isEnabled() && M5.Rtc.getDateTime(&dt)) {
    snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", dt.time.hours,
             dt.time.minutes, dt.time.seconds);
  }

  int batteryLevel = M5.Power.getBatteryLevel();
  batteryLevelSamples[batteryLevelIndex] = batteryLevel;
  batteryLevelIndex = (batteryLevelIndex + 1) % batteryLevelNumSamples;
  if (batteryLevelCount < batteryLevelNumSamples) {
    batteryLevelCount++;
  }

  uint32_t sum = 0;
  for (uint16_t i = 0; i < batteryLevelCount; i++) {
    sum += batteryLevelSamples[i];
  }
  uint8_t averageBatteryLevel = sum / batteryLevelCount;

  char batteryString[16];
  snprintf(batteryString, sizeof(batteryString), "%d%%", averageBatteryLevel);

  const char *modeString = presentationMode ? "Presentation" : "Normal";

  isCharging = averageBatteryLevel > previousAverageBatteryLevel;

  displayManager.showDefaultScreen(timeString, modeString, batteryString,
                                   isCharging, bleDevice.isConnected());

  previousAverageBatteryLevel = averageBatteryLevel;
}

void Application::setup() {
  auto cfg = M5.config();
  cfg.serial_baudrate = 115200;
  cfg.clear_display = true;
  cfg.internal_imu = true;
  cfg.internal_rtc = true;
  cfg.internal_spk = true;
  cfg.internal_mic = true;
  cfg.led_brightness = 255;
  M5.begin(cfg);

  pinMode(buttonPin, INPUT_PULLUP);

  auto imu_type = M5.Imu.getType();
  if (imu_type == m5::imu_none) {
    M5_LOGI("IMU not found, stopping.");
    while (true) {
      delay(1);
    }
  }
  M5.Imu.begin();
  M5.Imu.setCalibration(0, 10, 0);
  M5.Power.begin();

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

  switch (currentState) {
  case AppState::Menu:
    updateMenu();

    if (millis() - menuLastInteractionTime > menuTimeout) {
      currentState = AppState::Default;
      displayManager.resetPreviousValues();
      updateDefaultScreen();
    }
    break;

  case AppState::SensitivityAdjustment:
    updateSensitivityAdjustment();
    break;

  case AppState::IMUDisplay:
    updateIMUDisplay();
    break;

  case AppState::Default:
  default:
    handleIMUData();
    handleButtonPress();

    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime > 950) {
      lastUpdateTime = millis();
      updateDefaultScreen();
    }

    if (rotaryHandler.isButtonPressed()) {
      currentState = AppState::Menu;
      menuLastInteractionTime = millis();
      lastRotaryPosition = rotaryHandler.getPosition();

      auto labels = menu.getAllLabels();
      displayManager.printMenuItems(labels, menu.getCurrentIndex());
    }
    break;
  }

  M5.Display.display();
}

void Application::handleIMUData() {
  static unsigned long previousTime = millis();
  unsigned long currentTime = millis();

  if (M5.Imu.update() && bleDevice.isConnected()) {
    float val[3];
    M5.Imu.getGyro(&val[0], &val[1], &val[2]);

    if (presentationMode) {
      if (inCooldown &&
          currentTime - cooldownStartTime > arrowCooldownDuration) {
        inCooldown = false;
        M5_LOGI("Cooldown ended");
      }

      if (!inCooldown) {
        if (!leftInitialDetected && val[1] <= leftInitialThreshold) {
          leftInitialDetected = true;
          leftIterationCount = 0;
          M5_LOGI("Left arrow: Initial threshold detected");
        } else if (leftInitialDetected) {
          leftIterationCount++;
          if (val[1] >= leftFinalThreshold) {
            bleDevice.write(0xD8);
            M5_LOGI("Left arrow triggered");
            inCooldown = true;
            cooldownStartTime = currentTime;
            leftInitialDetected = false;
            leftIterationCount = 0;
          } else if (leftIterationCount >= arrowDetectionMaxIterations) {
            leftInitialDetected = false;
            leftIterationCount = 0;
            M5_LOGI("Left arrow: Timeout");
          }
        }

        if (!rightInitialDetected && val[1] >= rightInitialThreshold) {
          rightInitialDetected = true;
          rightIterationCount = 0;
          M5_LOGI("Right arrow: Initial threshold detected");
        } else if (rightInitialDetected) {
          rightIterationCount++;
          if (val[1] <= rightFinalThreshold) {
            bleDevice.write(0xD7);
            M5_LOGI("Right arrow triggered");
            inCooldown = true;
            cooldownStartTime = currentTime;
            rightInitialDetected = false;
            rightIterationCount = 0;
          } else if (rightIterationCount >= arrowDetectionMaxIterations) {
            rightInitialDetected = false;
            rightIterationCount = 0;
            M5_LOGI("Right arrow: Timeout");
          }
        }
      }
    } else {
      unsigned long deltaMs = currentTime - previousTime;
      previousTime = currentTime;

      deltaMs = deltaMs == 0 ? 10 : deltaMs;

      int8_t movement_x =
          static_cast<int8_t>(-val[2] * deltaMs * sensitivity / 1000.0f);
      int8_t movement_y =
          static_cast<int8_t>(-val[0] * deltaMs * sensitivity / 1000.0f);

      bleDevice.move(movement_x, movement_y);
    }
  }
}

void Application::handleConnectionEvents() {
  if (bleDevice.isConnected()) {
    if (!wasConnected) {
      ledController.setBrightness(20);
      soundManager.playSoundSequence(&connectSequence);
      wasConnected = true;
    }
  } else if (wasConnected) {
    ledController.blink(ledBlinkInterval);
    soundManager.playSoundSequence(&disconnectSequence);
    wasConnected = false;
  }
}

void Application::handleButtonPress() {
  unsigned long currentTime = millis();

  if (M5.BtnB.wasPressed()) {
    resetCursor();
  }

  if (digitalRead(buttonPin) == LOW) {
    if (!leftClickPressed &&
        (currentTime - leftClickLastDebounceTime) > leftClickDebounceDelay) {
      bleDevice.mousePress(1);
      leftClickPressed = true;
      leftClickLastDebounceTime = currentTime;
    }
  } else if (leftClickPressed && (currentTime - leftClickLastDebounceTime) >
                                     leftClickDebounceDelay) {
    bleDevice.mouseRelease(1);
    leftClickPressed = false;
    leftClickLastDebounceTime = currentTime;
  }
}
