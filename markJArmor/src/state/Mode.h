#pragma once

#include <stdint.h>

#include "../subsystems/InputToggles.h"

// A Mode is a sustained, top-level behavior of the suit (STANDBY, ACTIVATED,
// ...). It is distinct from the per-pixel step a subsystem takes on each tick.
// The machine holds exactly one Mode at a time and switches between them via
// transition().
enum class Mode : uint8_t {
  STANDBY,    // idle: future rest state, entered only by input (stubbed)
  ACTIVATED,  // active demo: chest ring crawl runs continuously
  // Future modes (RANGER_READY, SONIC_BURST, ...): append here and wire in
  // mode::next() + Machine::update().
};

namespace mode {
  // Decide the next Mode from the current one, how long we have been in it,
  // and the current button/toggle input. Input overrides instantly; time
  // provides fallback transitions (e.g. auto-arm out of a future STANDBY).
  Mode next(Mode current, uint32_t elapsedInMode, const InputToggles& input);

  // Human-readable label for logging.
  const char* name(Mode m);
}  // namespace mode