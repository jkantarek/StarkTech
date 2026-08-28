#include "Mode.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM  // host: plain RAM-backed literals
#endif

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
  // True flash strings (PROGMEM): callers may read them with pgm_read_byte
  // (HUD) or print them flash-aware (FPSTR). On the host PROGMEM is empty and
  // the statics are RAM-backed, so plain reads work there.
  switch (m) {
    case Mode::STANDBY: {
      static const char kStandby[] PROGMEM = "STANDBY";
      return kStandby;
    }
    case Mode::ACTIVATED: {
      static const char kActivated[] PROGMEM = "ACTIVATED";
      return kActivated;
    }
    default: {
      static const char kUnknown[] PROGMEM = "?";
      return kUnknown;
    }
  }
}

}  // namespace mode