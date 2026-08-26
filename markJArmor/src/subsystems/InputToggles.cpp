#include "InputToggles.h"

#include <Arduino.h>

void InputToggles::setup() {
  // STUBBED: assign and configure input pins here once hardware exists.
  Serial.println(F("input: stub ok (no pins wired)"));
}

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