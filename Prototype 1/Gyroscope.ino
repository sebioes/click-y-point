#include "Modulino.h"
#include "Mouse.h"

ModulinoMovement movement;

float x;
float y;
float z;

float prevX = 0, prevY = 0, prevZ = 0; 
float pinchThreshold = 0.8;  
unsigned long lastPinchTime = 0;
unsigned long pinchDelay = 500; 

int mapX;
int mapY;

// Calibration button variables
const int buttonPin = 2;     
int buttonState = HIGH;     
int lastButtonState = HIGH;    
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // Debounce delay in milliseconds

float xOffset = 0, yOffset = 0, zOffset = 0; // Calibration offsets

// Second button (glove button) variables
const int glovePin = 4;         
int secondButtonState = HIGH;   
int lastSecondButtonState = HIGH;
unsigned long lastSecondDebounceTime = 0;
const unsigned long secondDebounceDelay = 50;

void setup() {
  Serial.begin(9600);
  Modulino.begin();
  movement.begin();
  Mouse.begin();

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(glovePin, INPUT_PULLUP);
}

void loop() {
  movement.update();

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // Reset the debouncing timer
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        Serial.println("Calibration Button pressed");
        calibrateSensor();
      }
    }
  }

  lastButtonState = reading;

  // Handle Second Button (Glove Button) with Debouncing
  int secondReading = digitalRead(glovePin);

  if (secondReading != lastSecondButtonState) {
    lastSecondDebounceTime = millis(); // Reset the debouncing timer
  }

  if ((millis() - lastSecondDebounceTime) > secondDebounceDelay) {
    if (secondReading != secondButtonState) {
      secondButtonState = secondReading;

      if (secondButtonState == LOW) {
        Serial.println("Second Button pressed");
        Mouse.click(MOUSE_LEFT);
      }
    }
  }

  lastSecondButtonState = secondReading;

  // Read and Apply Movement Data
  x = movement.getX() - xOffset; 
  y = movement.getY() - yOffset;
  z = movement.getZ() - zOffset;

  mapX = int(x * 6);
  mapY = int(y * 6);

  if (abs(mapX) > 1 || abs(mapY) > 1) {
    Mouse.move(-mapY, mapX, 0);
  }

  checkForPinch();

  // Update previous values
  prevX = x;
  prevY = y;
  prevZ = z;

  // Debugging Information
  Serial.print("Movement data: ");
  Serial.print("x ");
  Serial.print(x, 3);
  Serial.print("    y ");
  Serial.print(y, 3);
  Serial.print("    z ");
  Serial.println(z, 3);

  Serial.print("Mapped X: ");
  Serial.println(mapX);
  Serial.print("Mapped Y: ");
  Serial.println(mapY);

  delay(10);
}

