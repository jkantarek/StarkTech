// HeadsUpDisplay tests — the generic stats logger. Subsystems expose a
// `const char* status()` (nullptr = nothing to report) and Machine reports
// it under a tag; the HUD renders "<submodule>.status - <message>" (e.g.
// "chest.status - sweep") once per second through the (recording) Arduino
// mock Serial.
//
// Contract under test:
//   - update() logs immediately on the first call, then every kLogMs
//   - report(tag, nullptr) contributes no line
//   - re-reporting a tag replaces its status (latest wins, no duplicates)
//   - report() ignores anything beyond kMaxEntries distinct tags
#include <cstdio>
#include <string>

#include <Arduino.h>

#include "src/subsystems/HeadsUpDisplay/HeadsUpDisplay.h"

static int fails = 0;
#define CHECK(c)                                                         \
  do {                                                                   \
    if (!(c)) {                                                          \
      std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c);           \
      ++fails;                                                           \
    }                                                                    \
  } while (0)

int main() {
  HeadsUpDisplay hud;

  // First update logs immediately, in report order; nullptr status is
  // skipped entirely.
  SerialClass::clear();
  hud.report("alpha", "one");
  hud.report("beta", nullptr);
  hud.update(0);
  CHECK(SerialClass::log() == "alpha.status - one\n");

  // Re-reporting a tag replaces its status — one line, not two.
  SerialClass::clear();
  hud.report("alpha", "two");
  hud.update(1000);
  CHECK(SerialClass::log() == "alpha.status - two\n");

  // 999 ms after the last render: nothing (1999 - 1000 < 1000).
  SerialClass::clear();
  hud.update(1999);
  CHECK(SerialClass::log().empty());

  // Boundary 1000 ms later, but no new reports: entries were consumed by the
  // render at t=1000, so nothing stale renders.
  hud.update(2000);
  CHECK(SerialClass::log().empty());

  // A fresh report renders at the next boundary.
  hud.report("alpha", "three");
  hud.update(3000);
  CHECK(SerialClass::log() == "alpha.status - three\n");

  // When a status goes back to nullptr the tag's line disappears.
  SerialClass::clear();
  hud.report("alpha", nullptr);
  hud.update(4000);
  CHECK(SerialClass::log().empty());

  // Tag cap: only kMaxEntries distinct tags are kept; extras are dropped.
  {
    HeadsUpDisplay capped;
    SerialClass::clear();
    for (int i = 0; i < 9; ++i) {
      static const char* names[9] = {"a", "b", "c", "d", "e",
                                     "f", "g", "h", "i"};
      capped.report(names[i], "x");
    }
    capped.update(0);
    size_t lines = 0;
    const std::string& log = SerialClass::log();
    for (size_t i = 0; (i = log.find(".status - ", i)) != std::string::npos; ++i)
      ++lines;
    CHECK(lines == 8);  // not 9
  }

  if (fails == 0) {
    std::printf("hud_test PASS\n");
    return 0;
  }
  std::printf("hud_test: %d FAILURES\n", fails);
  return 1;
}