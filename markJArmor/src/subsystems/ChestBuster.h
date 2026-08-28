#pragma once

#include <stdint.h>

#include <Adafruit_NeoPixel.h>

#include "ChestBuster/Animations/LoaderBlueToGreen.h"

// PROGMEM is defined by Arduino.h on AVR; this header is also included by
// host tests that do not pull in Arduino.h first, so define it as empty here
// unless a TU already brought it in.
#ifdef __AVR__
#include <avr/pgmspace.h>
#elif !defined(PROGMEM)
#define PROGMEM
#endif

// Chest ring driver. update() is a non-blocking, millis()-gated render
// loop: it returns immediately unless a full step period (kStepMs) has
// elapsed, then advances the built-in loader sweep into the frame and writes
// setPixelColor(i, frame[i]) for every pixel, followed by show(). Other
// real-time work proceeds between steps. No pointers and no virtual
// interfaces — the sweep is a concrete member.
class ChestBuster {
 public:
  ChestBuster();

  // Initialize hardware (ring.begin, setBrightness) and start the sweep.
  // Call once from Machine::setup.
  void setup();

  // Advance the loader one frame if kStepMs has elapsed since the last
  // update, then render that frame to the ring.
  void update(const uint32_t now);

  // HUD status: static flash string, never nullptr while the ring is
  // attached. Read with pgm_read_byte (HUD_CHAR) on AVR.
  const char* status() const {
    static const char kStatus[] PROGMEM = "sweep";
    return kStatus;
  }

 private:
  static constexpr uint16_t kPin = 6;
  static constexpr uint16_t kPixels = 24;
  static constexpr uint32_t kStepMs = 100;  // step period in ms

  // Per-step color frame, sized for the ring.
  uint32_t _frame[kPixels];

  cb::LoaderBlueToGreen _loader;  // the chest sweep
  Adafruit_NeoPixel _ring;
  uint32_t _lastStepMs;
};