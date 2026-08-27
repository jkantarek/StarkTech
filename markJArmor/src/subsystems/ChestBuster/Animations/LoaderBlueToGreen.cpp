#include <Adafruit_NeoPixel.h>

#include "LoaderBlueToGreen.h"

namespace cb {

void LoaderBlueToGreen::begin(uint16_t pixels) {
  _pixels = pixels;
  _i = 0;
}

bool LoaderBlueToGreen::step(uint32_t* frame) {
  if (_i >= _pixels) {
    // Pass finished: blank the whole ring and restart the sweep.
    for (uint16_t x = 0; x < _pixels; ++x) {
      frame[x] = 0;
    }
    _i = 0;
    return true;
  }

  // Paint the gradient tail: pixels 0.._i keep their per-index color, the
  // rest of the frame is dark until the sweep reaches them. Color packing is
  // the library's own (Adafruit_NeoPixel::Color, static — white stays 0).
  for (uint16_t x = 0; x < _pixels; ++x) {
    frame[x] = (x <= _i)
                   ? Adafruit_NeoPixel::Color(0, (uint8_t)(x * 10),
                                              (uint8_t)(255 - (x * 10)))
                   : 0;
  }
  ++_i;
  return false;
}

}  // namespace cb