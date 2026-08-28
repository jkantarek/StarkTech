#include "Mode.h"

namespace {

// How long STANDBY holds before auto-activating. STANDBY is only reachable
// via future input (nothing enters it today); the timer keeps the machine
// from wedging if it is ever entered, and fireHeld() jumps in instantly.
constexpr uint32_t kStandbyHoldMs = 3000;

}  // namespace

namespace mode {

Mode next(Mode current, const uint32_t elapsedInMode, const InputToggles& input) {
  switch (current) {
    case Mode::STANDBY:
      // Input override first; otherwise the hold timer fires.
      if (input.fireHeld() || elapsedInMode >= kStandbyHoldMs) {
        return Mode::ACTIVATED;
      }
      return Mode::STANDBY;

    case Mode::ACTIVATED:
      // ACTIVATED persists indefinitely; leaving is reserved for future
      // input-driven transitions (see InputToggles).
      (void)elapsedInMode;
      return Mode::ACTIVATED;

    default:
      return Mode::STANDBY;
  }
}

const char* name(const Mode m) {
  switch (m) {
    case Mode::STANDBY:   return "STANDBY";
    case Mode::ACTIVATED: return "ACTIVATED";
    default:              return "?";
  }
}

}  // namespace mode