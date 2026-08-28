#pragma once

#include <stdint.h>

// Reads physical buttons/switches and turns them into the toggle flags the
// state machine consults, plus exposes the "state control" surface for
// forcing/blocking mode transitions.
//
// STUBBED: no pins are wired yet — every input reports its default (off)
// state so the machine can be driven by time alone. Wire real pins here when
// the harness exists.
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

  // Reserved state-control surface (mode lock, cycle, ...); stubbed to 0.
  uint8_t reservedControlFlags() const;

 private:
  // Placeholder pin reserve for the fire button; unused while stubbed.
  static constexpr uint8_t kFirePin = 2;
};