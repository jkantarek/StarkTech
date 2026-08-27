#pragma once

#include <stdint.h>

#include <Adafruit_NeoPixel.h>

#include "ChestBuster/Animations/Animation.h"

// Home of the ring animation that used to live in the sketch's loop(). The
// former delay(1000) blocking step is now a millis()-based, non-blocking tick:
// each update() returns immediately unless a full step period has elapsed, so
// other real-time work proceeds between steps.
//
// setup() receives an cb::Animation to attach (the loader sweep, "off", a
// future one, ...); update() is a dumb render loop — it asks the animation
// for the next frame and writes setPixelColor(i, frame[i]) for every pixel,
// then show(). Selection of which animation to run is a one-line setup choice,
// so swapping the chest effect never touches the update path.
class ChestBuster {
 public:
  ChestBuster();

  // Initialize hardware (ring.begin, blank first frame), attach the
  // animation to run, and report progress. Call once from Machine::setup.
  void setup(cb::Animation* animation);

  // Advance the animation one frame if STEP_MS has elapsed since the last
  // update, then render that frame to the ring.
  void update(const uint32_t now);

 private:
  static constexpr uint16_t kPin = 6;
  static constexpr uint16_t kPixels = 24;
  static constexpr uint32_t kStepMs = 100;  // former delay(1000)

  // Per-step color frame, sized for the ring.
  uint32_t _frame[kPixels];

  Adafruit_NeoPixel _ring;
  cb::Animation* _animation;  // set in setup(); never null on the board
  uint32_t _lastStepMs;
};
