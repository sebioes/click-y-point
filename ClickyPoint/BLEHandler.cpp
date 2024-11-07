#include "BLEHandler.h"

BLEHandler::BLEHandler(const char *deviceName, const char *serviceName,
                       const uint16_t maxX, const uint16_t maxY)
    : bleDevice(deviceName, serviceName), maxX(maxX), maxY(maxY) {}

void BLEHandler::begin() { bleDevice.begin(true, true); }

bool BLEHandler::isConnected() { return bleDevice.isConnected(); }

void BLEHandler::move(int8_t x, int8_t y) { bleDevice.move(x, y); }

void BLEHandler::setPosition(uint16_t x, uint16_t y) {
  bleDevice.setPosition(x, y, maxX, maxY);
}

void BLEHandler::click(uint8_t b) { bleDevice.click(b); }

void BLEHandler::press(uint8_t b) { bleDevice.mousePress(b); }

void BLEHandler::release(uint8_t b) { bleDevice.mouseRelease(b); }

void BLEHandler::write(uint8_t c) { bleDevice.write(c); }
