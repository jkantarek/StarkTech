#pragma once

#include "Animation.h"

namespace cb {

// All pixels off, every frame. The blunt "lights out" animation: attach
// instead of a loader and the ring stays black — no state, no pass.
class AnimationOff : public Animation {
 public:
  void begin(uint16_t pixels) override;
  bool step(uint32_t* frame) override;
};

}  // namespace cb