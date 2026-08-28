#pragma once

#include <stdint.h>

#include "../subsystems/ChestBuster.h"
#include "../subsystems/ChestBuster/Animations/LoaderBlueToGreen.h"
#include "../subsystems/InputToggles.h"
#include "../subsystems/SonicCannon.h"
#include "Mode.h"

// Central state machine. Replaces the sketch's loop():
//   - owns the current Mode and when it was entered,
//   - samples inputs, evaluates Mode::next() and switches on change,
//   - drives only the subsystems relevant to the active Mode.
// The loop is non-blocking: update() never blocks (a one-off delay(500) for
// the boot block lives in setup()). Subsystems tick off millis(); timers
// survive millis() wraparound via unsigned arithmetic.
class Machine {
 public:
  // Serial + subsystem hardware init. Call once from setup().
  void setup();

  // One machine tick. Call from loop() as fast as possible.
  void update();

  // Test/diagnostic access: the currently active mode.
  Mode mode() const;

 private:
  void enterMode(const Mode next, const uint32_t now);

  // Boot straight into the demo: the crawl runs gapless from power-up.
  // STANDBY is a future rest state, reachable only via input.
  Mode _mode = Mode::ACTIVATED;
  uint32_t _modeStartMs = 0;  // millis() when the current mode was entered

  InputToggles _input;
  ChestBuster _chest;
  SonicCannon _sonic;
  cb::LoaderBlueToGreen _loader;  // chest effect, selected at setup
};