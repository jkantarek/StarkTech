#pragma once

#include <stdint.h>

// Placeholder for the suit's sound features (SFX, tones, future audio
// board). STUBBED: no hardware attached, no behavior yet. The machine
// already holds and ticks it, so wiring sound in means filling setup()/
// update().
class SonicCannon {
 public:
  // Initialize any audio hardware. Call once from Machine::setup.
  // STUBBED: no-op.
  void setup();

  // Called every machine tick with the current time while the owning mode is
  // active. STUBBED: no-op.
  void update(const uint32_t now);

  // HUD status: nullptr while no sound hardware exists.
  const char* status() const { return nullptr; }
};