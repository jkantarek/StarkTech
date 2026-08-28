// HeadsUpDisplay tests — the generic stats logger. The HUD is the ONE place
// Serial is used for routine reporting; it renders "[HUD] <name>: <fields>"
// lines through the (recording) Arduino mock Serial, once per 1000 ms, for
// every attached subsystem's status().
//
// Contract under test:
//   - update() logs immediately on the first call, then every kLogMs
//   - each attached subsystem contributes a line, in attach order
//   - subsystems report through the StatusSink seam — the HUD never needs to
//     know a subsystem's concrete type
//   - attach() ignores anything beyond kMaxEntries
#include <cstdio>
#include <string>
#include <vector>

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

// Fake subsystem: emits tag + num through the status seam.
class FakeSub : public Subsystem {
 public:
  FakeSub() : _tag("x"), _num(0) {}
  FakeSub(const char* tag, uint32_t num) : _tag(tag), _num(num) {}
  void status(StatusSink& out) const override {
    out.field("tag", _tag);
    out.field("num", _num);
  }

 private:
  const char* _tag;
  uint32_t _num;
};

static size_t countLines() {
  const std::string& log = SerialClass::log();
  size_t n = 0;
  for (size_t i = 0; (i = log.find("[HUD]", i)) != std::string::npos; ++i)
    ++n;
  return n;
}

int main() {
  FakeSub alpha("alpha", 7);
  FakeSub beta("beta", 42);
  HeadsUpDisplay hud;
  hud.attach("alpha", &alpha);
  hud.attach("beta", &beta);

  // First update logs immediately.
  SerialClass::clear();
  hud.update(0);
  CHECK(countLines() == 2);
  CHECK(SerialClass::log() ==
        "[HUD] alpha: tag=alpha num=7\n[HUD] beta: tag=beta num=42\n");

  // 999 ms later: no new log (999 - 0 < 1000).
  hud.update(999);
  CHECK(countLines() == 2);

  // At exactly 1000 ms: next log fires.
  hud.update(1000);
  CHECK(countLines() == 4);
  hud.update(1999);
  CHECK(countLines() == 4);
  hud.update(2000);
  CHECK(countLines() == 6);

  // No subsystems attached -> no logging, no crash.
  HeadsUpDisplay empty;
  SerialClass::clear();
  empty.update(0);
  CHECK(SerialClass::log().empty());

  // Attach cap: the 9th fake is dropped.
  {
    FakeSub extra[8];  // 8 more, but only kMaxEntries-2 slots free
    HeadsUpDisplay capped;
    for (int i = 0; i < 8; ++i) capped.attach("x", &extra[i]);
    capped.attach("alpha", &alpha);
    capped.attach("beta", &beta);  // dropped silently
    SerialClass::clear();
    capped.update(0);
    CHECK(countLines() == 8);  // not 10
  }

  if (fails == 0) {
    std::printf("hud_test PASS\n");
    return 0;
  }
  std::printf("hud_test: %d FAILURES\n", fails);
  return 1;
}