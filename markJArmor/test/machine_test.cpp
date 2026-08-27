// End-to-end machine tests — the whole firmware (real Machine/Mode/
// ChestBuster/InputToggles/SonicCannon sources, untouched) run against the
// injected clock and the recording ring. The ONLY mocks in the entire suite
// are test/mocks/Arduino.h (F, millis, Serial) and test/mocks/
// Adafruit_NeoPixel.h (records writes). See Makefile.
//
// Contract under test (src/subsystems/ChestBuster.cpp refactored: setup takes
// an Animation, update renders it):
//   - setup: begin + setBrightness + attach the loader animation only — NO
//     recorded writes, no boot transmission
//   - fast cadence (kStepMs = 10): each tick renders ONE FULL FRAME — the
//     update loop calls ring.setPixelColor(i, frame[i]) for every index
//     0..23, then show(). So every tick records 24 writes, indices 0..23.
//   - 25 ticks (t=100..340) = one loader pass (24 build ticks) + the wrap
//     tick that blanks the frame. Total writes = 25 frames * 24 px = 600.
//   - the pass restarts at index 0 after the wrap; mode stays ACTIVATED
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

  // Boot: no recorded writes — begin()/setBrightness() aren't logged, no boot
  // transmission in this build, and attaching the animation writes nothing.
  CHECK(Adafruit_NeoPixel::log.empty());

  // Drive 25 ticks at the 10 ms cadence (t=100..340). Each tick renders a
  // full frame: 24 pixel writes (indices 0..23). 25 * 24 = 600 writes total.
  for (uint32_t t = 100; t <= 340; t += 10) {
    g_now = t;
    m.update();
  }
  CHECK(Adafruit_NeoPixel::log.size() == 600);

  // Every frame writes all 24 pixels in order 0..23 (setPixelColor loop).
  for (size_t f = 0; f < 25; ++f)
    for (size_t k = 0; k < 24; ++k)
      CHECK(Adafruit_NeoPixel::log[f * 24 + k] == (int)k);

  // Machine parked in the demo mode — no pause-generating transitions.
  CHECK(m.mode() == Mode::ACTIVATED);

  if (fails == 0) {
    std::printf("machine_test PASS\n");
    return 0;
  }
  std::printf("machine_test: %d FAILURES\n", fails);
  return 1;
}