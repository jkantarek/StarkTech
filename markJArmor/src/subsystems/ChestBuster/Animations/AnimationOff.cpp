#include "AnimationOff.h"

namespace cb {

void AnimationOff::begin(uint16_t pixels) {
  _pixels = pixels;
}

bool AnimationOff::step(uint32_t* frame) {
  for (uint16_t i = 0; i < _pixels; ++i) {
    frame[i] = 0;
  }
  return false;  // the off state never wraps
}

}  // namespace cb