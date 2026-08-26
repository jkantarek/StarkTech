// End-to-end machine tests — the whole firmware (real Machine/Mode/
// ChestBuster/InputToggles/SonicCannon sources, untouched) run against the
// injected clock and the recording ring. The ONLY mocks in the entire suite
// are test/mocks/Arduino.h (F, millis, Serial) and test/mocks/
// Adafruit_NeoPixel.h (records writes). See Makefile.
//
// Contract under test (reference-identical build — Adafruit strandtest_nodelay
// colorWipe model, see src/subsystems/ChestBuster.cpp):
//   - setup transmits NOTHING recordable: begin() is malloc+memset, the single
//     boot show() sends the all-zero buffer; the mock only logs clear() and
//     setPixelColor(), neither of which setup calls
//   - each 1000 ms tick writes exactly one pixel then shows (write-then-show),
//     covering every index 0..23 and wrapping cleanly
//   - the machine stays in ACTIVATED (no time-gated pause)
#include <cstdio>
#include <vector>

#include "src/state/Machine.h"

std::vector<int> Adafruit_NeoPixel::log;

static unsigned long g_now = 0;
unsigned long test_now_ms() { return g_now; }

static int fails = 0;
#define CHECK(c)                                                         \
  do {                                                                   \
    if (!(c)) {                                                          \
      std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c);           \
      ++fails;                                                           \
    }                                                                    \
  } while (0)

int main() {
  Machine m;
  m.setup();

  // Boot: no recorded writes — begin()/show() are not logged by the mock.
  CHECK(Adafruit_NeoPixel::log.empty());

  // 25 stepping ticks (1100..25100 ms). One write per tick:
  // indices 0,1,...,23, then wrap to 0 => log[k] == k % 24 for k in 0..24.
  for (uint32_t t = 1100; t <= 25100; t += 1000) {
    g_now = t;
    m.update();
  }
  CHECK(Adafruit_NeoPixel::log.size() == 25);
  for (size_t k = 0; k < Adafruit_NeoPixel::log.size(); k++)
    CHECK(Adafruit_NeoPixel::log[k] == (int)(k % 24));

  // Machine parked in the demo mode — no pause-generating transitions.
  CHECK(m.mode() == Mode::ACTIVATED);

  if (fails == 0) {
    std::printf("machine_test PASS\n");
    return 0;
  }
  std::printf("machine_test: %d FAILURES\n", fails);
  return 1;
}