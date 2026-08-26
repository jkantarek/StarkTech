#pragma once

#include <stdint.h>

#include "../subsystems/ChestBuster.h"
#include "../subsystems/InputToggles.h"
#include "../subsystems/SonicCannon.h"
#include "Mode.h"

// Central state machine. Replaces the sketch's loop():
//   - owns the current Mode and when it was entered,
//   - samples inputs, evaluates Mode::next() and switches on change,
//   - drives only the subsystems relevant to the active Mode.
// Everything is non-blocking (no delay): subsystems tick off millis().
// Timers survive millis() wraparound via unsigned arithmetic.
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

  Mode _mode = Mode::STANDBY;
  uint32_t _modeStartMs = 0;  // millis() when the current mode was entered

  InputToggles _input;
  ChestBuster _chest;
  SonicCannon _sonic;
};