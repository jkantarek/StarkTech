#include "SonicCannon.h"

#include <Arduino.h>

void SonicCannon::setup() {
  // STUBBED: initialize sound hardware once it exists.
  Serial.println(F("sonic: stub ok (no hardware)"));
}

void SonicCannon::update(const uint32_t now) {
  (void)now;  // STUBBED: drive sound features here once hardware exists.
}