void calibrateSensor() {
  // Capture the current sensor readings as the neutral position
  movement.update();
  xOffset = movement.getX();
  yOffset = movement.getY();
  zOffset = movement.getZ();

  Serial.println("Calibration complete.");
  Serial.print("xOffset: ");
  Serial.println(xOffset, 3);
  Serial.print("yOffset: ");
  Serial.println(yOffset, 3);
  Serial.print("zOffset: ");
  Serial.println(zOffset, 3);
}