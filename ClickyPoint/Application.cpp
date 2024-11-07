#include "Application.h"
#include "SoundManager.h"

// Define the melodies
const ToneDuration Application::connectMelody[] = {
    {600, 120}, {0, 30}, {400, 120}, {0, 30}, {800, 120}};

const ToneDuration Application::disconnectMelody[] = {
    {400, 150}, {0, 30}, {300, 150}};

const ToneDuration Application::resetCursorMelody[] = {{100, 50}};

unsigned long lastDebounceTime = 0;
const uint16_t debounceDelay = 100;
bool leftClickPressed = false;

unsigned long arrowLastDebounce = 0;
const uint16_t arrowDebounceDelay = 400;
bool rightArrowTriggered = false;
bool leftArrowTriggered = false;
uint8_t arrowIterationCount = 0;
const uint8_t maxArrowIterations = 50;
const float arrowSensitivity = 1.0; 

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
      wasConnected(true), presentationMode(false) {}

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
}

void Application::loop() {
  M5.update();
  unsigned long currentTime = millis();
  // Handle IMU data
  if (imuHandler.update()) {
    IMUData data = imuHandler.getData();

    if (presentationMode) {
      if (arrowIterationCount > maxArrowIterations) {
        rightArrowTriggered = false;
        leftArrowTriggered = false;
        arrowIterationCount = 0;
      }

      if (!rightArrowTriggered && !leftArrowTriggered && data.accel_x <= - arrowSensitivity) {
        rightArrowTriggered = true;
        arrowIterationCount = 0; // Start counting iterations
      }

      // Check for left arrow condition
      if (!leftArrowTriggered && !leftArrowTriggered && data.accel_x >= arrowSensitivity) {
        leftArrowTriggered = true;
        arrowIterationCount = 0; // Start counting iterations
      }

      // If rightArrowTriggered, monitor for return to < -0.1
      if (rightArrowTriggered) {
        arrowIterationCount++;
        if (data.accel_x > -0.1 && arrowIterationCount <= maxArrowIterations && currentTime - arrowLastDebounce > arrowDebounceDelay) {
          Serial.println("Right Arrow Triggered");
          bleHandler.write(0xD7); // Trigger right arrow
          rightArrowTriggered = false;
          arrowIterationCount = 0;
          arrowLastDebounce = currentTime;
        }
      }

      // If leftArrowTriggered, monitor for return to > 0.1
      if (leftArrowTriggered) {
        arrowIterationCount++;
        if (data.accel_x < 0.1 || arrowIterationCount <= maxArrowIterations && currentTime - arrowLastDebounce > arrowDebounceDelay) {
          Serial.println("Left Arrow Triggered");
          bleHandler.write(0xD8); // Trigger left arrow
          leftArrowTriggered = false;
          arrowIterationCount = 0;
          arrowLastDebounce = currentTime;
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
      if (bleHandler.isConnected()) {
        ledController.setBrightness(100);
        bleHandler.move(movement_x, movement_y);
      }
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
    if (!leftClickPressed && (currentTime - lastDebounceTime) > debounceDelay) {
      bleHandler.press(1); // Hold left click
      leftClickPressed = true;
      lastDebounceTime = currentTime;
    }
  } else { // Button is released
    if (leftClickPressed && (currentTime - lastDebounceTime) > debounceDelay) {
      bleHandler.release(1); // Release left click
      leftClickPressed = false;
      lastDebounceTime = currentTime;
    }
  }

  // Update sound manager
  soundManager.update();
}
