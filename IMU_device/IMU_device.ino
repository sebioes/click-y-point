#include <M5Unified.h>
#include "BleDevice.h"
#include <math.h>

BleDevice bleDevice("Click y Point", "Click y Point");
unsigned long previousTime = 0;
float scalingFactor = 30.0; // mouse sensitivity 
bool posToggle = 1; // stupid solution to get repeated absolute position

// LED Blinking Variables
const unsigned long BLINK_INTERVAL = 500; // milliseconds
unsigned long ledPreviousTime = 0;
bool ledState = false;

bool wasConnected = false;

void setup() {
  auto cfg = M5.config();
  cfg.led_brightness = 255;
  M5.begin(cfg);

  // Initialize Serial Monitor
  Serial.begin(115200);

  // Check if IMU is initialized
  const char* name;
  auto imu_type = M5.Imu.getType();
  switch (imu_type) {
    case m5::imu_none: name = "not found"; break;
    case m5::imu_sh200q: name = "sh200q"; break;
    case m5::imu_mpu6050: name = "mpu6050"; break;
    case m5::imu_mpu6886: name = "mpu6886"; break;
    case m5::imu_mpu9250: name = "mpu9250"; break;
    case m5::imu_bmi270: name = "bmi270"; break;
    default: name = "unknown"; break;
  };
  Serial.printf("IMU: %s\n", name);

  if (imu_type == m5::imu_none) {
    Serial.println("IMU not found, stopping.");
    for (;;) { delay(1); }
  }

  M5.Speaker.setVolume(255);

  // Initialize BLE device
  bleDevice.begin();

  previousTime = millis();
}

void loop() {
  M5.update();

  if (M5.Imu.update()) {
    // Get IMU data
    auto data = M5.Imu.getImuData();

    // Get current time
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - previousTime) / 1000.0;  // in seconds
    previousTime = currentTime;

    // Avoid division by zero
    if (deltaTime <= 0) {
      deltaTime = 0.01;  // assume 10ms if zero
    }

    // Compute movement based on gyroscope data
    int movement_x = int(-data.gyro.z * deltaTime * scalingFactor);
    int movement_y = int(-data.gyro.x * deltaTime * scalingFactor);

    // Apply thresholds to ignore small movements
    int threshold = 1;  // adjust as needed
    if (abs(movement_x) < threshold) movement_x = 0;
    if (abs(movement_y) < threshold) movement_y = 0;

    // Only send movement if there is movement and BLE device is connected
    if (bleDevice.isConnected() && (movement_x != 0 || movement_y != 0)) {
      M5.Power.setLed(100);
      bleDevice.move(movement_x, movement_y);
    }

    // Debugging output
    // Serial.printf("Gyro Z: %f, Gyro X: %f, Move X: %d, Move Y: %d\n",
    // data.gyro.z, data.gyro.x, movement_x, movement_y);
  }

  unsigned long currentTime = millis();

  bool isConnected = bleDevice.isConnected();

  // Detect connection status change
  if (isConnected != wasConnected) {
    if (isConnected) {
      // Device just connected
      Serial.println("BLE Connected");
      M5.Power.setLed(80);
      playConnectSound();
    } else {
      // Device just disconnected
      Serial.println("BLE Disconnected");
      playDisconnectSound();
    }
    wasConnected = isConnected;
  }

  if (!isConnected) {
    // Advertising: Blink LED
    if (currentTime - ledPreviousTime >= BLINK_INTERVAL) {
      ledPreviousTime = currentTime;
      ledState = !ledState; // Toggle LED state
      if (ledState) {
        M5.Power.setLed(255); // LED on at full brightness
      } else {
        M5.Power.setLed(0);   // LED off
      }
    }
  }

  // Check if the button was pressed to reset cursor position
  if (M5.BtnPWR.wasPressed()) {
    // Reset cursor to starting position
    if (bleDevice.isConnected()) {
      M5.Speaker.tone(100, 50);
      if (posToggle) {
        bleDevice.setPosition(960, 540, 1920, 1080);
      } else {
        bleDevice.setPosition(961, 541, 1920, 1080);
      }
      posToggle = !posToggle;
    }
  }
}

void playConnectSound() {
  M5.Speaker.tone(600, 120);
  delay(250);
  M5.Speaker.tone(400, 120);
  delay(250);
  M5.Speaker.tone(800, 120);
}

void playDisconnectSound() {
  M5.Speaker.tone(400, 150);
  delay(300);
  M5.Speaker.tone(300, 150);
}
