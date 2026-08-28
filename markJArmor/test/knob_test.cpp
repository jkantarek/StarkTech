// KnobA6 + Mappings tests — the analog knob driver and its range mapping.
// Hardware seam: the Arduino mock's analogRead(A6) is scripted per-sample via
// test_analog_read(), exactly like the injected clock for millis().
//
// Contract under test:
//   - update() samples on a 20 ms cadence (monotonic test clock); the FIRST
//     sample fires immediately at t=0, then every 20 ms — 50 samples complete
//     the boot scan, whose min/max are the observed extrema
//   - raw() is the latest sample; min/max keep tracking live after the scan,
//     so a wider sweep rescales; rescan() clears and re-arms
//   - mapping::percent: raw in [min,max] -> 0..100, clamped; degenerate
//     (min==max) range -> 0, never divide-by-zero
//   - status(): nullptr while unscanned, "ready" once the scan completes
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include <Arduino.h>

#include "src/subsystems/InputToggles/knobA6/KnobA6.h"
#include "src/subsystems/InputToggles/Mappings/Range.h"

static int fails = 0;
#define CHECK(c)                                                         \
  do {                                                                   \
    if (!(c)) {                                                          \
      std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c);           \
      ++fails;                                                           \
    }                                                                    \
  } while (0)

// Scripted analog source: each analogRead() pops the next scripted ADC value.
static std::vector<uint16_t> g_script;
static size_t g_script_i = 0;
int test_analog_read(uint8_t) {
  if (g_script_i >= g_script.size()) return 0;
  return g_script[g_script_i++];
}

int main() {
  // --- mapping::percent ---
  CHECK(mapping::percent(500, 100, 900) == 50);
  CHECK(mapping::percent(100, 100, 900) == 0);   // at min -> 0
  CHECK(mapping::percent(50, 100, 900) == 0);    // below min -> clamped
  CHECK(mapping::percent(900, 100, 900) == 100); // at max -> 100
  CHECK(mapping::percent(950, 100, 900) == 100); // above max -> clamped
  CHECK(mapping::percent(200, 500, 500) == 0);   // degenerate: no div-by-zero

  // --- boot scan: min/max over the sweep, live tracking, rescan ---
  {
    KnobA6 k;
    k.setup();
    CHECK(!k.scanned());

    g_script = {300, 900, 100, 500, 800, 200, 700, 400, 600, 350,
                250, 850, 150, 550, 750, 450, 650, 950, 50, 1000,
                320, 910, 110, 510, 810, 210, 710, 410, 610, 360,
                260, 860, 160, 560, 760, 460, 660, 960, 60, 990,
                330, 920, 120, 520, 820, 220, 720, 420, 620, 370};
    g_script_i = 0;

    uint32_t now = 0;
    // 10 samples at t=0..180 (first call samples immediately).
    for (int s = 0; s < 10; ++s) {
      k.update(now);
      now += 20;
    }
    CHECK(!k.scanned());    // 10 of 50
    CHECK(k.raw() == 350);  // script[9]
    CHECK(k.minRaw() == 100);  // min of samples 0..9
    CHECK(k.maxRaw() == 900);  // max of samples 0..9

    // Finish the sweep: 40 more samples, t=200..980 -> 50 total.
    for (int s = 0; s < 40; ++s) {
      k.update(now);
      now += 20;
    }
    CHECK(k.scanned());
    CHECK(k.raw() == 370);       // script[49]
    CHECK(k.minRaw() == 50);     // global min
    CHECK(k.maxRaw() == 1000);   // global max
    CHECK(k.percent() == 33);    // (370-50)*100/(1000-50) = 33.68 -> 33
    CHECK(k.status() != nullptr);
    CHECK(k.status()[0] == 'r'); // "ready" once scanned

    // Live tracking beyond the scanned bounds rescales min/max.
    g_script = {20, 1023};
    g_script_i = 0;
    k.update(now);  // t=1000 -> reads 20
    now += 20;
    k.update(now);  // t=1020 -> reads 1023
    CHECK(k.minRaw() == 20);
    CHECK(k.maxRaw() == 1023);

    // Burst gate: update() before the next sample due (t < 1040) is a no-op.
    k.update(now);  // t=1020
    CHECK(k.raw() == 1023);

    // rescan clears bounds + scan state and re-arms the sweep; status goes
    // back to nullptr until the scan completes again.
    k.rescan();
    CHECK(!k.scanned());
    CHECK(k.status() == nullptr);
    now += 20;
    k.update(now);     // t=1040: samples (script exhausted -> 0)
    CHECK(k.raw() == 0);
    CHECK(k.percent() == 0);  // unscanned -> 0
  }

  if (fails == 0) {
    std::printf("knob_test PASS\n");
    return 0;
  }
  std::printf("knob_test: %d FAILURES\n", fails);
  return 1;
}