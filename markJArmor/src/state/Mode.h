#pragma once

#include <stdint.h>

#include "../subsystems/InputToggles.h"

// A Mode is a sustained, top-level behavior of the suit (STANDBY, ACTIVATED,
// ...). It is distinct from the per-pixel step a subsystem takes on each tick.
// The machine holds exactly one Mode at a time and switches between them via
// transition().
enum class Mode : uint8_t {
  STANDBY,    // resting: no subsystem active
  ACTIVATED,  // reaction on: chest ring reacts
  // Future modes (RANGER_READY, SONIC_BURST, ...) can be appended here and
  // wired in transition() + Machine::update().
};

namespace mode {
  // Decide the next Mode from the current one, how long we have been in it,
  // and the current button/toggle input. Time drives the demo "breath" beat;
  // input can override (e.g. a fire button) instantly.
  Mode next(Mode current, uint32_t elapsedInMode, const InputToggles& input);

  // Human-readable label for logging.
  const char* name(Mode m);
}  // namespace mode