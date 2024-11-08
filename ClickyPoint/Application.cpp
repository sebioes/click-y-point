#include "Application.h"
#include "SoundManager.h"
#include "M5StickCPlus2.h"

// Define the melodies
const ToneDuration Application::connectMelody[] = {
    {600, 120}, {0, 30}, {400, 120}, {0, 30}, {800, 120}};

const ToneDuration Application::disconnectMelody[] = {
    {400, 150}, {0, 30}, {300, 150}};

const ToneDuration Application::resetCursorMelody[] = {{100, 50}};

unsigned long leftClickLastDebounceTime = 0;
const uint16_t leftClickDebounceDelay = 100;
bool leftClickPressed = false;

// Cooldown settings
const uint16_t cooldownDuration = 500; // milliseconds
unsigned long cooldownStartTime = 0;
bool inCooldown = false;

// Gesture detection settings
const uint16_t maxIterations = 80;

// Left arrow thresholds
const float leftInitialThreshold = -400.0;
const float leftFinalThreshold = 300.0;

// Right arrow thresholds
const float rightInitialThreshold = 700.0;
const float rightFinalThreshold = 400.0;

// Variables to track gesture progress
bool leftInitialDetected = false;
uint16_t leftIterationCount = 0;

bool rightInitialDetected = false;
uint16_t rightIterationCount = 0;

// Define the sound sequences
const SoundSequence Application::connectSequence = {
    Application::connectMelody, sizeof(connectMelody) / sizeof(ToneDuration)};
const SoundSequence Application::disconnectSequence = {
    Application::disconnectMelody,
    sizeof(disconnectMelody) / sizeof(ToneDuration)};
const SoundSequence Application::resetCursorSequence = {
    Application::resetCursorMelody,
    sizeof(resetCursorMelody) / sizeof(ToneDuration)};

Application::Application()
    : bleHandler("Click y Point", "Click y Point", 1920, 1080), posToggle(true),
      wasConnected(true), presentationMode(false), rotaryHandler(33, 32, 0) {}

void Application::setupMenu() {
    // Add menu item with dynamic label for presentation mode
    menu.addItem(
        [this]() -> const char* {
            return presentationMode ? "Mode: Presentation" : "Mode: Normal";
        },
        [this]() { 
            togglePresentationMode(); 
        }
    );
    
    // Add static menu items
    menu.addItem("Reset Cursor", [this]() { resetCursor(); });
    menu.addItem("Show IMU Data", [this]() { showIMUData(); });
}

void Application::togglePresentationMode() {
    presentationMode = !presentationMode;
    displayManager.printText(presentationMode ? "Mode: Presentation" : "Mode: Normal");
}

void Application::resetCursor() {
    if (bleHandler.isConnected()) {
        // soundManager.playSoundSequence(&resetCursorSequence); // TODO: Add sound
        bleHandler.setPosition(960, 540);
    }
}

void Application::showIMUData() {
    if (imuHandler.update()) {
        IMUData data = imuHandler.getData();
        displayManager.printIMUData(data.gyro_x, data.gyro_y, data.gyro_z);
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
        size_t count;
        const char** labels = menu.getAllLabels(count);
        displayManager.printMenuItems(labels, count, menu.getCurrentIndex());
    }
    
    if (rotaryHandler.isButtonPressed()) {
        menu.select();
    }
}

void Application::setup() {
  // Initialize M5
  auto cfg = M5.config();
  cfg.led_brightness = 255;
  M5.begin(cfg);

  pinMode(26, INPUT_PULLUP);

  // Initialize Serial
  Serial.begin(115200);

  // Initialize IMU
  if (!imuHandler.initialize()) {
    Serial.println("IMU not found, stopping.");
    while (true) {
      delay(1);
    }
  }
  Serial.printf("IMU initialized.\n");

  // Initialize Sound Manager
  soundManager.begin();

  // Initialize BLE
  bleHandler.begin();

  // Initialize the rotary encoder
  rotaryHandler.initialize();

  // Initialize Display Manager
  displayManager.begin();

  // Initialize the menu
  setupMenu();

  // Show initial menu items
  size_t count;
  const char** labels = menu.getAllLabels(count);
  displayManager.printMenuItems(labels, count, menu.getCurrentIndex());
}

void Application::loop() {
  M5.update();
  unsigned long currentTime = millis();
  // Handle IMU data
  if (imuHandler.update() && bleHandler.isConnected()) {
    IMUData data = imuHandler.getData();

    if (presentationMode) {
      float gyro_y = data.gyro_y;

      // --------- Cooldown Management ---------
      if (inCooldown) {
        if (currentTime - cooldownStartTime > cooldownDuration) {
          inCooldown = false;
          Serial.println("Cooldown ended");
        }
      }

      // If not in cooldown, proceed to detect gestures
      if (!inCooldown) {
        // --------- Left Arrow Detection ---------
        if (!leftInitialDetected) {
          if (gyro_y <= leftInitialThreshold) {
            leftInitialDetected = true;
            leftIterationCount = 0;
            Serial.println("Left arrow: Initial threshold detected");
          }
        } else {
          leftIterationCount++;
          if (gyro_y >= leftFinalThreshold) {
            // Trigger left arrow
            bleHandler.write(0xD8);
            Serial.println("Left arrow triggered");
            inCooldown = true;
            cooldownStartTime = currentTime;

            // Reset detection
            leftInitialDetected = false;
            leftIterationCount = 0;
          } else if (leftIterationCount >= maxIterations) {
            // Timeout
            leftInitialDetected = false;
            leftIterationCount = 0;
            Serial.println("Left arrow: Timeout");
          }
        }

        // --------- Right Arrow Detection ---------
        if (!rightInitialDetected) {
          if (gyro_y >= rightInitialThreshold) {
            rightInitialDetected = true;
            rightIterationCount = 0;
            Serial.println("Right arrow: Initial threshold detected");
          }
        } else {
          rightIterationCount++;
          if (gyro_y <= rightFinalThreshold) {
            // Trigger right arrow
            bleHandler.write(0xD7);
            Serial.println("Right arrow triggered");
            inCooldown = true;
            cooldownStartTime = currentTime;

            // Reset detection
            rightInitialDetected = false;
            rightIterationCount = 0;
          } else if (rightIterationCount >= maxIterations) {
            // Timeout
            rightInitialDetected = false;
            rightIterationCount = 0;
            Serial.println("Right arrow: Timeout");
          }
        }
      }
    } else {
      static unsigned long previousTime = currentTime;
      unsigned long deltaMs = currentTime - previousTime;
      previousTime = currentTime;

      // Prevent division by zero
      if (deltaMs == 0)
        deltaMs = 10; // Assume 10ms

      // Compute movement using integer math
      int8_t movement_x =
          static_cast<int8_t>(-data.gyro_z * deltaMs * 35.0f / 1000.0f);
      int8_t movement_y =
          static_cast<int8_t>(-data.gyro_x * deltaMs * 35.0f / 1000.0f);

      // Send movement if connected and movement exists
      bleHandler.move(movement_x, movement_y);
    }
  }

  // Handle connection events
  if (bleHandler.isConnected()) {
    if (!wasConnected) {
      ledController.setBrightness(80);
      soundManager.playSoundSequence(&connectSequence);
      wasConnected = true;
    }
  } else {
    if (wasConnected) {
      ledController.blink(BLINK_INTERVAL);
      soundManager.playSoundSequence(&disconnectSequence);
      wasConnected = false;
    }
  }

  // Update LED controller
  ledController.update();

  // Handle button press
  if (M5.BtnPWR.wasPressed() && bleHandler.isConnected()) {
    soundManager.playSoundSequence(&resetCursorSequence);
    if (posToggle) {
      bleHandler.setPosition(960, 540);
    } else {
      bleHandler.setPosition(961, 541);
    }
    posToggle = !posToggle;
  }
  if (M5.BtnA.wasPressed()) {
    presentationMode = !presentationMode;
  }
  if (digitalRead(26) == LOW) { // Button is pressed
    if (!leftClickPressed && (currentTime - leftClickLastDebounceTime) > leftClickDebounceDelay) {
      bleHandler.press(1); // Hold left click
      leftClickPressed = true;
      leftClickLastDebounceTime = currentTime;
    }
  } else { // Button is released
    if (leftClickPressed && (currentTime - leftClickLastDebounceTime) > leftClickDebounceDelay) {
      bleHandler.release(1); // Release left click
      leftClickPressed = false;
      leftClickLastDebounceTime = currentTime;
    }
  }

  // Update sound manager
  soundManager.update();

  // Update rotary encoder and menu
  rotaryHandler.update();
  updateMenu();
}
