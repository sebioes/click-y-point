#ifndef ESP32_BLE_DEVICE_MOUSE_H
#define ESP32_BLE_DEVICE_MOUSE_H

#include "BleDevice.h"

// Mouse buttons
#define MOUSE_LEFT    1
#define MOUSE_RIGHT   2
#define MOUSE_MIDDLE  4
#define MOUSE_BACK    8
#define MOUSE_FORWARD 16
#define MOUSE_ALL     (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE | MOUSE_BACK | MOUSE_FORWARD) // For compatibility with the Mouse library

// Report IDs
#define MOUSE_RELATIVE_ID  0x01
#define MOUSE_ABSOLUTE_ID  0x02

static const uint8_t _hidReportDescriptorMouse[] PROGMEM = {
  // Report 1: Relative Movement
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x02,        // Usage (Mouse)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (Pointer)
  0xA1, 0x00,        //   Collection (Physical)
  0x85, MOUSE_RELATIVE_ID, //     Report ID (1)
  // Buttons (Left, Right, Middle)
  0x05, 0x09,        //     Usage Page (Button)
  0x19, 0x01,        //     Usage Minimum (Button 1)
  0x29, 0x03,        //     Usage Maximum (Button 3)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x95, 0x03,        //     Report Count (3)
  0x75, 0x01,        //     Report Size (1)
  0x81, 0x02,        //     Input (Data,Var,Abs)
  // Padding
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x05,        //     Report Size (5)
  0x81, 0x03,        //     Input (Constant,Var,Abs)
  // X and Y (Relative)
  0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //     Usage (X)
  0x09, 0x31,        //     Usage (Y)
  0x09, 0x38,        //     Usage (Wheel)
  0x15, 0x81,        //     Logical Minimum (-127)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x95, 0x03,        //     Report Count (3)
  0x81, 0x06,        //     Input (Data,Var,Rel)
  // Horizontal Wheel
  0x05, 0x0C,        //     Usage Page (Consumer Devices)
  0x09, 0x38, 0x02,  //     Usage (AC Pan)
  0x15, 0x81,        //     Logical Minimum (-127)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x95, 0x01,        //     Report Count (1)
  0x81, 0x06,        //     Input (Data,Var,Rel)
  0xC0,              //   End Collection
  0xC0,              // End Collection

  // Report 2: Absolute Positioning
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x02,        // Usage (Mouse)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (Pointer)
  0xA1, 0x00,        //   Collection (Physical)
  0x85, MOUSE_ABSOLUTE_ID, //     Report ID (2)
  // Buttons (Left, Right, Middle)
  0x05, 0x09,        //     Usage Page (Button)
  0x19, 0x01,        //     Usage Minimum (Button 1)
  0x29, 0x03,        //     Usage Maximum (Button 3)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x95, 0x03,        //     Report Count (3)
  0x75, 0x01,        //     Report Size (1)
  0x81, 0x02,        //     Input (Data,Var,Abs)
  // Padding
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x05,        //     Report Size (5)
  0x81, 0x03,        //     Input (Constant,Var,Abs)
  // X and Y (Absolute)
  0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //     Usage (X)
  0x09, 0x31,        //     Usage (Y)
  0x16, 0x00, 0x00,  //     Logical Minimum (0)
  0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
  0x36, 0x00, 0x00,  //     Physical Minimum (0)
  0x46, 0xFF, 0x7F,  //     Physical Maximum (32767)
  0x75, 0x10,        //     Report Size (16)
  0x95, 0x02,        //     Report Count (2)
  0x81, 0x02,        //     Input (Data,Var,Abs)
  0xC0,              //   End Collection
  0xC0               // End Collection
};

#endif

