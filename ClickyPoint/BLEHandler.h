#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include "BleDevice.h"

class BLEHandler {
public:
  BLEHandler(const char *deviceName, const char *serviceName,
             const uint16_t maxX, const uint16_t maxY);
  void begin();
  void update();
  bool isConnected();
  void move(int8_t x, int8_t y);
  void setPosition(uint16_t x, uint16_t y);
  void click(uint8_t b);
  void press(uint8_t b);
  void release(uint8_t b);
  void write(uint8_t c);

private:
  BleDevice bleDevice;
  uint16_t maxX;
  uint16_t maxY;
};

#endif
