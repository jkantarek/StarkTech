#pragma once

#include <stdint.h>

// Placeholder for the suit's sound features (SFX, tones, future audio board).
// STUBBED: no hardware attached, no behavior yet. The machine already holds
// and ticks it so wiring sound in is a matter of filling update() and adding
// a Mode (see Mode.h / Machine.h).
class SonicCannon {
 public:
  // Initialize any audio hardware. Call once from Machine::setup.
  // STUBBED: no-op.
  void setup();

  // Called every machine tick with the current time while the owning mode is
  // active. STUBBED: no-op.
  void update(const uint32_t now);

 private:
  // Reserved: volume / gain state, current tone, etc.
  uint8_t _reserved = 0;
};