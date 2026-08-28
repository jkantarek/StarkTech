#pragma once

#include <stdint.h>

#include <Adafruit_NeoPixel.h>

#include "ChestBuster/Animations/Animation.h"

// Chest ring driver. update() is a non-blocking, millis()-gated render loop:
// it returns immediately unless a full step period (kStepMs) has elapsed,
// then asks the attached animation for the next frame and writes
// setPixelColor(i, frame[i]) for every pixel, followed by show(). Other
// real-time work proceeds between steps.
//
// setup() receives an cb::Animation to attach (the loader sweep, "off", a
// future one, ...). Selection of which animation to run is a one-line setup
// choice, so swapping the chest effect never touches the update path.
class ChestBuster {
 public:
  ChestBuster();

  // Initialize hardware (ring.begin, setBrightness), attach the
  // animation to run, and report progress. Call once from Machine::setup.
  void setup(cb::Animation* animation);

  // Advance the animation one frame if STEP_MS has elapsed since the last
  // update, then render that frame to the ring.
  void update(const uint32_t now);

 private:
  static constexpr uint16_t kPin = 6;
  static constexpr uint16_t kPixels = 24;
  static constexpr uint32_t kStepMs = 100;  // step period in ms

  // Per-step color frame, sized for the ring.
  uint32_t _frame[kPixels];

  Adafruit_NeoPixel _ring;
  cb::Animation* _animation;  // set in setup(); never null on the board
  uint32_t _lastStepMs;
};
