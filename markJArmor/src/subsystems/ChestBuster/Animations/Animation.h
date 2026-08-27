#pragma once

#include <stdint.h>

// Pure-logic base for ChestBuster ring animations. An animation owns its
// per-pixel state and, per step(), fills a full frame — one packed uint32
// color per LED — that ChestBuster::update then writes to the ring with
// setPixelColor(i, frame[i]) for every pixel, followed by show(). The render
// loop stays animation-agnostic: selection happens once at ChestBuster::setup.
//
// No Adafruit/Arduino dependency (unlike the ring itself), so animations are
// host-testable like the other pure-logic modules (state/Mode, InputToggles).
namespace cb {

class Animation {
 public:
  virtual ~Animation() {}

  // Configure for a ring of `pixels` LEDs and reset to frame 0. Called once
  // from ChestBuster::setup when the animation is attached.
  virtual void begin(uint16_t pixels) = 0;

  // Advance one step and fill `frame[0.._pixels)` with this frame's colors.
  // Frame colors are Adafruit-packed (see Adafruit_NeoPixel::Color; the
  // ring is NEO_GRBW so white stays 0). Returns true when the pass wrapped
  // (restarted its first frame).
  virtual bool step(uint32_t* frame) = 0;

 protected:
  uint16_t _pixels = 0;  // ring size, set by begin()
};

}  // namespace cb