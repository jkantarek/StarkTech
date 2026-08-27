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
  void setBrightness(uint8_t) {}
  void clear() { log.push_back(-1); }
  void show() {}
  void setPixelColor(uint16_t n, uint32_t c) { log.push_back((int)n); }
  // Static, like the real lib: (w<<24)|(r<<16)|(g<<8)|b regardless of strip
  // wire order (here the ring is NEO_GRBW so white = 0).
  static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
  }

  static std::vector<int> log;  // -1 = clear, 0.. = pixel index written
};