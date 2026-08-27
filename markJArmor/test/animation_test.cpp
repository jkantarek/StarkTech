// Unit tests for the ChestBuster animation classes (TDD-first): an animation
// owns per-pixel state and, per step(), fills a full frame (one color per
// LED) with setPixelColor-compatible packed uint32. Pure logic — no Arduino
// or ring mocks needed, so this TU links only the animation sources.
//
// Contract under test:
//   - cb::rgb(): packs R,G,B into the ring's uint32 format. Adafruit_NeoPixel
//     stores pixels WRGB-packed regardless of strip wire order:
//     (w<<24)|(r<<16)|(g<<8)|b — white unused (0) by these animations.
//   - AnimationOff: every frame is all-zeros and never reports a wrap.
//   - LoaderBlueToGreen: left-to-right sweep leaves a blue->green gradient
//     (same palette as the original loop: R=0, G=i*10, B=255-(i*10)); when
//     the sweep passes the last LED the frame blanks and the pass restarts
//     (signals wrap).
#include <cstdint>
#include <cstdio>

#include <Adafruit_NeoPixel.h>

#include "src/subsystems/ChestBuster/Animations/Animation.h"
#include "src/subsystems/ChestBuster/Animations/AnimationOff.h"
#include "src/subsystems/ChestBuster/Animations/LoaderBlueToGreen.h"

static int fails = 0;
#define CHECK(c)                                                         \
  do {                                                                   \
    if (!(c)) {                                                          \
      std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c);           \
      ++fails;                                                           \
    }                                                                    \
  } while (0)

// The real library's Color() is static; our mock mirrors its
// (w<<24)|(r<<16)|(g<<8)|b packing so expected values are computed with the
// same call the animation uses — no reinvention.
static uint32_t loaderGradient(uint16_t i) {
  return Adafruit_NeoPixel::Color(0, (uint8_t)(i * 10u), (uint8_t)(255u - i * 10u));
}

int main() {
  // --- Adafruit_NeoPixel::Color packing sanity (mock = real formula) ---
  CHECK(Adafruit_NeoPixel::Color(0, 0, 255) == 0x000000FFul);   // blue
  CHECK(Adafruit_NeoPixel::Color(0, 230, 25) == 0x0000E619ul);  // G=0xE6,B=0x19

  // --- AnimationOff ---
  {
    cb::AnimationOff off;
    uint32_t frame[24];
    off.begin(24);
    for (int s = 0; s < 3; ++s) {
      bool wrapped = off.step(frame);
      CHECK(!wrapped);
      for (int i = 0; i < 24; ++i) CHECK(frame[i] == 0);
    }
  }

  // --- LoaderBlueToGreen: build a pass, then wrap ---
  {
    cb::LoaderBlueToGreen loader;
    uint32_t frame[24];
    loader.begin(24);

    // Step 1 lights pixel 0 only; the rest of the frame stays off.
    bool w1 = loader.step(frame);
    CHECK(!w1);
    CHECK(frame[0] == loaderGradient(0));
    for (int i = 1; i < 24; ++i) CHECK(frame[i] == 0);

    // Steps 2..24 complete the pass: every pixel lit with its gradient.
    for (int s = 2; s <= 24; ++s) {
      bool w = loader.step(frame);
      CHECK(!w);
      for (int i = 0; i < 24; ++i)
        CHECK(frame[i] == (i <= (s - 1) ? loaderGradient(i) : 0));
    }

    // Step 25: the pass wrapped — frame blanks, wrap reported, cursor reset.
    bool wrapped = loader.step(frame);
    CHECK(wrapped);
    for (int i = 0; i < 24; ++i) CHECK(frame[i] == 0);

    // Step 26: new pass restarts at pixel 0.
    bool w2 = loader.step(frame);
    CHECK(!w2);
    CHECK(frame[0] == loaderGradient(0));
    for (int i = 1; i < 24; ++i) CHECK(frame[i] == 0);
  }

  if (fails == 0) {
    std::printf("animation_test PASS\n");
    return 0;
  }
  std::printf("animation_test: %d FAILURES\n", fails);
  return 1;
}