void checkForPinch(){
    // Check for pinch motion by detecting a sudden acceleration spike
  if ((abs(x - prevX) > pinchThreshold || abs(y - prevY) > pinchThreshold || abs(z - prevZ) > pinchThreshold) &&
      (millis() - lastPinchTime > pinchDelay)) {
    // Register a pinch action if detected
    Serial.println("Pinch detected!");
    lastPinchTime = millis();  // Reset debounce timer
    Mouse.click(MOUSE_LEFT);
  }
}