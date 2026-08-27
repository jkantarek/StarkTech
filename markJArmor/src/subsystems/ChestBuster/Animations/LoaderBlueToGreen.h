#pragma once

#include "Animation.h"

namespace cb {

// The chest "loader_blue_to_green" sweep: a left-to-right pass paints a
// blue->green gradient (R=0, G=i*10, B=255-(i*10)) over the ring, one LED per
// step, then blanks and restarts. Reproduces the original single-pixel loop:
// at step N the first N pixels hold their gradient, the rest stay off.
class LoaderBlueToGreen : public Animation {
 public:
  void begin(uint16_t pixels) override;
  bool step(uint32_t* frame) override;

 private:
  // Count of pixels painted so far in this pass (0 = none, == _pixels when
  // the pass just completed and the next step blanks + restarts).
  uint16_t _i = 0;
};

}  // namespace cb