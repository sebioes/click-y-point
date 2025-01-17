#ifndef BATTERY_CHARGING_H
#define BATTERY_CHARGING_H

constexpr uint8_t battery_charging_png[] = {
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20,
  0x08, 0x04, 0x00, 0x00, 0x00, 0xd9, 0x73, 0xb2, 0x7f, 0x00, 0x00, 0x00,
  0x20, 0x63, 0x48, 0x52, 0x4d, 0x00, 0x00, 0x7a, 0x26, 0x00, 0x00, 0x80,
  0x84, 0x00, 0x00, 0xfa, 0x00, 0x00, 0x00, 0x80, 0xe8, 0x00, 0x00, 0x75,
  0x30, 0x00, 0x00, 0xea, 0x60, 0x00, 0x00, 0x3a, 0x98, 0x00, 0x00, 0x17,
  0x70, 0x9c, 0xba, 0x51, 0x3c, 0x00, 0x00, 0x00, 0x02, 0x62, 0x4b, 0x47,
  0x44, 0x00, 0xff, 0x87, 0x8f, 0xcc, 0xbf, 0x00, 0x00, 0x00, 0x09, 0x70,
  0x48, 0x59, 0x73, 0x00, 0x00, 0x0e, 0xc3, 0x00, 0x00, 0x0e, 0xc3, 0x01,
  0xc7, 0x6f, 0xa8, 0x64, 0x00, 0x00, 0x00, 0x07, 0x74, 0x49, 0x4d, 0x45,
  0x07, 0xe8, 0x0b, 0x0a, 0x00, 0x0c, 0x07, 0x1c, 0x7f, 0x38, 0x95, 0x00,
  0x00, 0x01, 0x2a, 0x49, 0x44, 0x41, 0x54, 0x48, 0xc7, 0xed, 0x93, 0xbd,
  0x4a, 0x03, 0x41, 0x14, 0x46, 0xcf, 0xc4, 0xa0, 0x44, 0xd3, 0x2a, 0x08,
  0xfe, 0x20, 0xd8, 0x1b, 0x10, 0x1b, 0x5f, 0x40, 0x82, 0x16, 0x36, 0x16,
  0x16, 0x56, 0x76, 0x76, 0x01, 0x5f, 0x42, 0x04, 0xab, 0x14, 0xa2, 0x88,
  0x95, 0x95, 0x2f, 0x60, 0xa5, 0x68, 0xb0, 0xd4, 0x4a, 0xc1, 0x68, 0x63,
  0xa3, 0x18, 0x41, 0xb0, 0x11, 0x0c, 0x59, 0xf7, 0xb3, 0xd8, 0x08, 0x29,
  0x72, 0xaf, 0xc9, 0x8a, 0x5d, 0x4e, 0x37, 0xdc, 0xf9, 0xce, 0xde, 0x99,
  0xbb, 0x03, 0x3d, 0xfe, 0x4c, 0x00, 0xe5, 0x58, 0x67, 0x96, 0xa1, 0x8e,
  0x33, 0x5f, 0x54, 0x39, 0x0a, 0x0f, 0xcd, 0x95, 0x86, 0x75, 0xa7, 0xee,
  0xf9, 0xd0, 0xe2, 0x8f, 0xe0, 0x30, 0x45, 0x5c, 0x92, 0x6a, 0xca, 0x03,
  0x64, 0x58, 0x70, 0xdb, 0x7d, 0xa1, 0x6a, 0x54, 0x46, 0x98, 0x4f, 0x3a,
  0xf0, 0xf8, 0xd4, 0x8c, 0x8a, 0x66, 0xb5, 0xa2, 0x4d, 0x0d, 0xf8, 0x82,
  0x12, 0x68, 0xc3, 0xdd, 0x71, 0xe0, 0x09, 0x4e, 0x14, 0x40, 0x65, 0x57,
  0xf0, 0x6e, 0x0b, 0x5e, 0x35, 0x0a, 0xa0, 0x33, 0x57, 0x10, 0x59, 0x82,
  0x58, 0x4b, 0xcd, 0x29, 0x15, 0xb5, 0xa2, 0xe3, 0xee, 0x05, 0xe5, 0xd6,
  0x0b, 0xd7, 0x4e, 0xb7, 0x82, 0x7b, 0xe5, 0x5a, 0xe2, 0x05, 0x35, 0x6c,
  0x41, 0xb6, 0xed, 0x8c, 0xb3, 0x2c, 0x2b, 0xe2, 0x34, 0xbc, 0x81, 0xfa,
  0xd8, 0xa3, 0xfd, 0x2e, 0x7e, 0xfb, 0x0f, 0xa6, 0x01, 0x54, 0xf2, 0x2f,
  0x31, 0x48, 0x86, 0x39, 0x66, 0x30, 0xd4, 0x35, 0xc1, 0x2d, 0x79, 0xef,
  0x61, 0x65, 0xcc, 0xd2, 0x73, 0xa8, 0x03, 0xbb, 0x6e, 0x1c, 0x9e, 0xec,
  0x23, 0x54, 0x40, 0xab, 0xf2, 0x69, 0x68, 0x0d, 0x45, 0x46, 0xf1, 0x52,
  0xe3, 0xaa, 0xb9, 0xf1, 0x2d, 0x4d, 0x01, 0xba, 0x51, 0x5a, 0xe6, 0x92,
  0xe7, 0xbc, 0x4f, 0x3a, 0x2e, 0xb8, 0x4a, 0xc6, 0x98, 0xd1, 0xb6, 0x79,
  0x0c, 0x9b, 0x73, 0x8d, 0x25, 0x9e, 0x00, 0xa0, 0x49, 0x0a, 0xf4, 0x77,
  0xfc, 0x6d, 0xf1, 0xc8, 0x75, 0x88, 0x53, 0x76, 0xde, 0xe3, 0x1f, 0xf8,
  0x06, 0x73, 0x5a, 0x42, 0x34, 0x81, 0x8c, 0x52, 0x0a, 0x00, 0x00, 0x00,
  0x25, 0x74, 0x45, 0x58, 0x74, 0x64, 0x61, 0x74, 0x65, 0x3a, 0x63, 0x72,
  0x65, 0x61, 0x74, 0x65, 0x00, 0x32, 0x30, 0x32, 0x34, 0x2d, 0x31, 0x31,
  0x2d, 0x31, 0x30, 0x54, 0x30, 0x30, 0x3a, 0x31, 0x30, 0x3a, 0x35, 0x37,
  0x2b, 0x30, 0x30, 0x3a, 0x30, 0x30, 0x29, 0xd7, 0x6a, 0x85, 0x00, 0x00,
  0x00, 0x25, 0x74, 0x45, 0x58, 0x74, 0x64, 0x61, 0x74, 0x65, 0x3a, 0x6d,
  0x6f, 0x64, 0x69, 0x66, 0x79, 0x00, 0x32, 0x30, 0x32, 0x34, 0x2d, 0x31,
  0x31, 0x2d, 0x31, 0x30, 0x54, 0x30, 0x30, 0x3a, 0x31, 0x30, 0x3a, 0x35,
  0x37, 0x2b, 0x30, 0x30, 0x3a, 0x30, 0x30, 0x58, 0x8a, 0xd2, 0x39, 0x00,
  0x00, 0x00, 0x28, 0x74, 0x45, 0x58, 0x74, 0x64, 0x61, 0x74, 0x65, 0x3a,
  0x74, 0x69, 0x6d, 0x65, 0x73, 0x74, 0x61, 0x6d, 0x70, 0x00, 0x32, 0x30,
  0x32, 0x34, 0x2d, 0x31, 0x31, 0x2d, 0x31, 0x30, 0x54, 0x30, 0x30, 0x3a,
  0x31, 0x32, 0x3a, 0x30, 0x37, 0x2b, 0x30, 0x30, 0x3a, 0x30, 0x30, 0x43,
  0x8a, 0x2d, 0xbf, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58, 0x74, 0x53,
  0x6f, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x77, 0x77, 0x77, 0x2e,
  0x69, 0x6e, 0x6b, 0x73, 0x63, 0x61, 0x70, 0x65, 0x2e, 0x6f, 0x72, 0x67,
  0x9b, 0xee, 0x3c, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
  0xae, 0x42, 0x60, 0x82
};
constexpr uint16_t battery_charging_png_len = 640;
constexpr uint8_t battery_charging_png_width = 32;
constexpr uint8_t battery_charging_png_height = 32;
#endif
