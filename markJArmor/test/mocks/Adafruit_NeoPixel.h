#pragma once
// Host-side stand-in for the real Adafruit_NeoPixel library, recording every
// strip write for assertions: setPixelColor(n) appends n, clear() appends -1.
// Same constant values as the installed lib (v1.15.5). The rest of the suite
// uses the real firmware sources untouched.
#include <stdint.h>
#include <vector>

#define NEO_GRBW ((3 << 6) | (1 << 4) | (0 << 2) | (2))
#define NEO_KHZ800 0x0000

typedef uint32_t neoPixelType;

class Adafruit_NeoPixel {
 public:
  Adafruit_NeoPixel(uint16_t, uint16_t, neoPixelType) {}
  void begin() {}
  void clear() { log.push_back(-1); }
  void show() {}
  void setPixelColor(uint16_t n, uint32_t) { log.push_back((int)n); }
  uint32_t Color(uint8_t, uint8_t, uint8_t) { return 0; }

  static std::vector<int> log;  // -1 = clear, 0.. = pixel index written
};