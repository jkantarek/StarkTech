#include "Mode.h"

namespace {

// How long the suit rests in STANDBY before "breathing" into ACTIVATED on its
// own, so the demo is time-driven even with no button wired. A real fire
// toggle jumps in instantly regardless of this window.
constexpr uint32_t kStandbyHoldMs = 3000;

// How long the reaction (chest ring animation) persists once entered.
constexpr uint32_t kActivatedHoldMs = 4000;

}  // namespace

namespace mode {

Mode next(Mode current, const uint32_t elapsedInMode, const InputToggles& input) {
  switch (current) {
    case Mode::STANDBY:
      // Input override first; otherwise the time-based demo beat.
      if (input.fireHeld() || elapsedInMode >= kStandbyHoldMs) {
        return Mode::ACTIVATED;
      }
      return Mode::STANDBY;

    case Mode::ACTIVATED:
      if (elapsedInMode >= kActivatedHoldMs) {
        return Mode::STANDBY;
      }
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