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

  // Initialize hardware (ring.begin). Call once from Machine::setup.
  void begin();

  // Advance the animation if STEP_MS has elapsed since the last step.
  void update(const uint32_t now);

 private:
  static constexpr uint16_t kPin = 6;
  static constexpr uint16_t kPixels = 24;
  static constexpr uint32_t kStepMs = 1000;  // former delay(1000)

  Adafruit_NeoPixel _ring;
  uint32_t _lastStepMs;
  uint16_t _i;  // pixel being lit this pass, grows 1..kPixels then wraps
};