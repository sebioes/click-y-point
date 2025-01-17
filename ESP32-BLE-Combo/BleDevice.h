#ifndef ESP32_BLE_DEVICE_H
#define ESP32_BLE_DEVICE_H
#include "sdkconfig.h"

#include "NimBLECharacteristic.h"
#include "NimBLEHIDDevice.h"

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <NimBLEHIDDevice.h>

#include "HIDTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define BLE_LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* BLE_LOG_TAG = "BLEDevice";
#endif

#define BLEDevice                  NimBLEDevice
#define BLEServerCallbacks         NimBLEServerCallbacks
#define BLECharacteristicCallbacks NimBLECharacteristicCallbacks
#define BLEHIDDevice               NimBLEHIDDevice
#define BLECharacteristic          NimBLECharacteristic
#define BLEAdvertising             NimBLEAdvertising
#define BLEServer                  NimBLEServer

// Report IDs:
#define MOUSE_RELATIVE_ID  0x01
#define MOUSE_ABSOLUTE_ID  0x02
#define KEYBOARD_ID        0x03
#define MEDIA_KEYS_ID      0x04

#include "Print.h"

#include "BleDevice_Kbd.h"
#include "BleDevice_Mouse.h"

class BleDevice : public Print, public BLEServerCallbacks, public BLECharacteristicCallbacks
{
private:
  // mouse related
  BLEHIDDevice* hid_mouse;
  uint8_t _buttons; // mouse buttons
  BLECharacteristic* inputMouse;
  BLECharacteristic* inputMouseAbsolute; // For absolute positioning
  void buttons(uint8_t b);
  void rawAction(uint8_t msg[], char msgSize); // added 
  static void taskServer(void* pvParameter); // added 
  bool enableMouse = false;
  
  // keyboard related
  BLEHIDDevice* hid;
  BLECharacteristic* inputKeyboard;
  BLECharacteristic* outputKeyboard;
  BLECharacteristic* inputMediaKeys;
  // BLEAdvertising*    advertising;
  KeyReport          _keyReport;
  MediaKeyReport     _mediaKeyReport;
  std::string        deviceName;
  std::string        deviceManufacturer;
  uint8_t           _keyboardLedsStatus;
  uint8_t            batteryLevel;
  esp_power_level_t  pwrLevel;
  bool               connected = false;
  uint32_t           _delay_ms = 7;
  void delay_ms(uint64_t ms);
  bool enableKeyboard = false;

  uint16_t vid       = 0x05ac;
  uint16_t pid       = 0x820a;
  uint16_t version   = 0x0210;

public:
  // max power level ESP_PWR_LVL_N3=-3dB, ESP_PWR_LVL_N0=0dB, *ESP_PWR_LVL_P3=3dB, ESP_PWR_LVL_P6=6dB, ESP_PWR_LVL_P9=9dB
  BleDevice(std::string deviceName = "ESP32 Device", std::string deviceManufacturer = "Espressif", esp_power_level_t pwrLevel=ESP_PWR_LVL_P3, uint8_t batteryLevel=100 ) ;
  void init(void);
  void begin(bool _enableMouse=true, bool _enableKeyboard=true);
  void end(bool clearAll = false);
  void sendReport(KeyReport* keys);
  void sendReport(MediaKeyReport* keys);
  size_t press(uint8_t k);
  size_t press(const MediaKeyReport k);
  size_t release(uint8_t k);
  size_t release(const MediaKeyReport k);
  size_t write(uint8_t c);
  size_t write(const MediaKeyReport c);
  size_t write(const uint8_t *buffer, size_t size);
  void releaseAll(void);
  bool isConnected(void);
  bool getLedStatus(uint8_t led);
  void setBatteryLevel(uint8_t level);
  void setName(std::string deviceName);  
  void setDelay(uint32_t ms);

  void setVendorID(uint16_t vid);
  void setProductID(uint16_t pid);
  void setVersion(uint16_t version);

  // mouse specific
  void click(uint8_t b = MOUSE_LEFT);
  void move(int8_t x, int8_t y, signed char wheel = 0, signed char hWheel = 0);
  void setPosition(uint16_t x, uint16_t y, uint16_t width, uint16_t height); // Absolute movement
  void mousePress(uint8_t b = MOUSE_LEFT);   // press LEFT by default
  void mouseRelease(uint8_t b = MOUSE_LEFT); // release LEFT by default
  bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default

protected:
  virtual void onStarted(BLEServer *pServer);
  virtual void onConnect(BLEServer* pServer) override;
  virtual void onDisconnect(BLEServer* pServer) override;
  virtual void onWrite(BLECharacteristic* me) override;

};

#endif // ESP32_BLE_DEVICE_H
