// Mode transition tests — ZERO mocks. Compiles and links the real, untouched
// src/state/Mode.cpp + src/subsystems/InputToggles.cpp as raw C++ (neither
// includes Arduino.h). InputToggles is the real stub: fireHeld() == false.
//
// Contract under test (see Mode.cpp):
//   STANDBY  -> ACTIVATED when fireHeld() OR elapsedInMode >= 3000
//   ACTIVATED-> ACTIVATED always (no time-gated exit; leaving is input-driven)
#include <cstdio>

#include "src/state/Mode.h"
#include "src/subsystems/InputToggles.h"

static int fails = 0;
#define CHECK(c)                                                         \
  do {                                                                   \
    if (!(c)) {                                                          \
      std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c);           \
      ++fails;                                                           \
    }                                                                    \
  } while (0)

int main() {
  InputToggles input;  // real stub: nothing wired, fireHeld() == false

  // STANDBY: stays until the 3000 ms auto-arm boundary.
  CHECK(mode::next(Mode::STANDBY, 0, input) == Mode::STANDBY);
  CHECK(mode::next(Mode::STANDBY, 2999, input) == Mode::STANDBY);
  CHECK(mode::next(Mode::STANDBY, 3000, input) == Mode::ACTIVATED);

  // ACTIVATED: persists indefinitely — this is what fixed the 3 s pause.
  CHECK(mode::next(Mode::ACTIVATED, 0, input) == Mode::ACTIVATED);
  CHECK(mode::next(Mode::ACTIVATED, 3000, input) == Mode::ACTIVATED);
  CHECK(mode::next(Mode::ACTIVATED, 4294967295UL, input) == Mode::ACTIVATED);

  // Labels.
  CHECK(mode::name(Mode::STANDBY)[0] == 'S');
  CHECK(mode::name(Mode::ACTIVATED)[0] == 'A');

  if (fails == 0) {
    std::printf("mode_test PASS\n");
    return 0;
  }
  std::printf("mode_test: %d FAILURES\n", fails);
  return 1;
}