#pragma once

#include <stdint.h>

namespace cb {

// The chest "loader_blue_to_green" sweep: a left-to-right pass paints a
// blue->green gradient (R=0, G=i*10, B=255-(i*10)) over the ring, one LED
// per step, then blanks and restarts. Concrete class — no base, no vtable:
// ChestBuster owns one directly and calls step() each tick.
class LoaderBlueToGreen {
 public:
  void begin(uint16_t pixels);
  bool step(uint32_t* frame);

 private:
  uint16_t _pixels = 0;

  // Count of pixels painted so far in this pass (0 = none, == _pixels when
  // the pass just completed and the next step blanks + restarts).
  uint16_t _i = 0;
};

}  // namespace cb