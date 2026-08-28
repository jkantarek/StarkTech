#pragma once

#include <stdint.h>

// Reads physical buttons/switches and exposes the flags the state machine
// consults. STUBBED: no pins are wired yet — every input reports its default
// (off) state so the machine can be driven by time alone. Wire real pins
// here when the harness exists.
class InputToggles {
 public:
  // Initialize input pins and report progress. Call once from Machine::setup.
  // STUBBED: no pins wired yet; reports the stub state.
  void setup();

  // Sample the hardware inputs. Call once per machine tick.
  void update();

  // True while the fire/action button is held. STUB: always false.
  // When wired, this should override time-driven transitions in Mode::next.
  bool fireHeld() const;

  // HUD status: nullptr while nothing is wired.
  const char* status() const { return nullptr; }
};