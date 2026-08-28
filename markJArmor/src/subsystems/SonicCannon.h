#pragma once

#include <stdint.h>

// Sound subsystem: drives the speaker through the Mozzi synthesis library in
// HIFI dual-PWM mode (MOZZI_OUTPUT_2PIN_PWM) — audio on pins 11+12 (Timer1
// A/B), combined through the 3.9k/499k resistor ladder into the amp input.
// The machine ticks update(now, active) every loop; while active the cannon
// tone plays, otherwise Mozzi keeps pumping silence so the PWM output rests
// at 0 instead of freezing the last sample (DC click on mode change).
class SonicCannon {
 public:
  // Start Mozzi's audio engine and set the cannon tone. Call once from
  // Machine::setup. Does nothing on host builds (see SonicCannon.cpp).
  void setup();

  // Pump the Mozzi audio buffer; `active` selects tone vs silence.
  // Call every machine tick, in every mode.
  void update(const uint32_t now, bool active);

  // HUD status: "fire" while the tone plays, "off" otherwise (PROGMEM).
  const char* status() const;

 private:
  bool _active = false;
};