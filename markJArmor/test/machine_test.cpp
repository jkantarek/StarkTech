// End-to-end machine tests — the whole firmware (real Machine/Mode/
// ChestBuster/InputToggles/SonicCannon sources, untouched) run against the
// injected clock and the recording ring. The ONLY mocks in the entire suite
// are test/mocks/Arduino.h (F, millis, Serial) and test/mocks/
// Adafruit_NeoPixel.h (records writes). See Makefile.
//
// Contract under test (src/subsystems/ChestBuster.cpp as committed 4b4c779):
//   - setup: begin + setBrightness only — NO recorded writes, no boot show
//   - fast cadence (kStepMs = 10): each tick writes one pixel (gradient),
//     showing the full pass over indices 0..23
//   - on wrap (after pixel 23) the pixels are reset MANUALLY to 0,0,0 via a
//     write loop — the library's clear() is never used. Matches the code's
//     `x < kPixels` loop exactly: 24 writes at indices 0..23.
//   - the sweep restarts at index 0; mode stays ACTIVATED
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

  // Boot: no recorded writes — begin()/setBrightness()/show() aren't logged,
  // and there is no boot transmission in this build.
  CHECK(Adafruit_NeoPixel::log.empty());

  // Drive 25 ticks at the 10 ms cadence:
  //   t=100..330 -> pass 1: indices 0..23 (24 single-pixel writes)
  //   t=330      -> wrap: reset loop writes 0..23 (24 zero-writes)
  //   t=340      -> pass 2 starts: index 0
  // Total recorded writes = 24 + 24 + 1 = 49.
  for (uint32_t t = 100; t <= 340; t += 10) {
    g_now = t;
    m.update();
  }
  CHECK(Adafruit_NeoPixel::log.size() == 49);

  // Pass 1: one write per tick, indices 0..23 — every LED lit once.
  for (size_t k = 0; k < 24; k++)
    CHECK(Adafruit_NeoPixel::log[k] == (int)k);

  // Wrap reset: the manual 0,0,0 write loop, indices 0..23 (code uses
  // `x < kPixels`).
  for (size_t k = 0; k < 24; k++)
    CHECK(Adafruit_NeoPixel::log[24 + k] == (int)k);

  // Pass 2 restart at index 0.
  CHECK(Adafruit_NeoPixel::log[48] == 0);

  // Machine parked in the demo mode — no pause-generating transitions.
  CHECK(m.mode() == Mode::ACTIVATED);

  if (fails == 0) {
    std::printf("machine_test PASS\n");
    return 0;
  }
  std::printf("machine_test: %d FAILURES\n", fails);
  return 1;
}