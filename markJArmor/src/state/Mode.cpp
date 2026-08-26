#include "Mode.h"

namespace {

// How long STANDBY holds before auto-activating. STANDBY is only entered by
// future input (nothing does today), but if we ever land there this keeps the
// machine from wedging; a real fire toggle jumps in instantly.
constexpr uint32_t kStandbyHoldMs = 3000;

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
      // Persist: the old time-based return to STANDBY is what paused the
      // ring every few seconds. Leaving ACTIVATED is reserved for future
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