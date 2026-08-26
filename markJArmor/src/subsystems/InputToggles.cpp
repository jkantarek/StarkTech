#include "InputToggles.h"

void InputToggles::update() {
  // STUBBED: sample DigitalRead of kFirePin (+ debounce) here once hardware
  // is assigned. Nothing to do while inputs report their defaults.
}

bool InputToggles::fireHeld() const {
  return false;  // STUB: no button wired yet
}

uint8_t InputToggles::reservedControlFlags() const {
  return 0;  // STUB: state-control surface reserved
}