#pragma once

#include <stdint.h>

#include <Adafruit_NeoPixel.h>

// Home of the ring animation that used to live in the sketch's loop(). The
// former delay(1000) blocking step is now a millis()-based, non-blocking tick:
// each update() returns immediately unless a full step period has elapsed, so
// other real-time work proceeds between steps.
class ChestBuster {
 public:
  ChestBuster();

  // Initialize hardware (ring.begin, blank first frame) and report progress.
  // Call once from Machine::setup.
  void setup();

  // Advance the animation if STEP_MS has elapsed since the last step.
  void update(const uint32_t now);

 private:
  static constexpr uint16_t kPin = 6;
  static constexpr uint16_t kPixels = 24;
  static constexpr uint32_t kStepMs = 10;  // former delay(1000)

  Adafruit_NeoPixel _ring;
  uint32_t _lastStepMs;
  // Index of the pixel lit on the next tick (0..kPixels-1: every LED).
  // Wrapping resets to 0; the next pass overwrites the previous one
  // (reference colorWipe style — no clear, no blank pulse).
  uint16_t _i;
};
