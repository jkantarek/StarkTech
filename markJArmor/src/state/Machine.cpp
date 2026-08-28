#include "Machine.h"

#include <Arduino.h>

#ifdef __AVR__
#ifndef FPSTR
// Older AVR cores lack FPSTR; it is a reinterpret of a PROGMEM char pointer
// as the flash-string handle Arduino's Serial can print directly.
#define FPSTR(p) (reinterpret_cast<const __FlashStringHelper*>(p))
#endif
#endif

namespace {

// HUD tags: PROGMEM on AVR so the HUD's flash reads (HUD_CHAR) work; the
// host mock defines PROGMEM as empty so these are plain RAM literals there.
const char kTagMode[] PROGMEM = "mode";
const char kTagChest[] PROGMEM = "chest";
const char kTagInput[] PROGMEM = "input";
const char kTagSonic[] PROGMEM = "sonic";

}  // namespace

void Machine::setup() {
  Serial.begin(9600);
  Serial.println(F("start"));

  _chest.setup();   // prints its own progress
  _sonic.setup();   // starts Mozzi (pins 11+12 hifi audio)

  _mode = Mode::ACTIVATED;
  _modeStartMs = millis();
  Serial.println(F("machine: ready"));

  // Drain TX and pause once so the host receives the full boot block before
  // update() starts (setup-only; the loop never blocks).
  Serial.flush();
  delay(500);
}

void Machine::update() {
  _input.update();  // STUB: samples no pins yet

  const uint32_t now = millis();
  const Mode next = mode::next(_mode, now - _modeStartMs, _input);
  if (next != _mode) {
    enterMode(next, now);
  }

  // Drive only the subsystems that belong to the active mode.
  switch (_mode) {
    case Mode::ACTIVATED:
      _chest.update(now);
      _sonic.update(now, true);  // cannon tone
      break;

    case Mode::STANDBY:
    default:
      // Pump silence in rest modes so the PWM output rests at 0 (no DC
      // click on mode change).
      _sonic.update(now, false);
      break;
  }

  // Feed the HUD once per tick; it renders every kLogMs. Subsystems with
  // nothing to report (nullptr) are skipped by report().
  _hud.report(kTagMode, mode::name(_mode));
  _hud.report(kTagChest, _chest.status());
  _hud.report(kTagInput, _input.status());
  _hud.report(kTagSonic, _sonic.status());
  _hud.update(now);
}

Mode Machine::mode() const {
  return _mode;
}

void Machine::enterMode(const Mode next, const uint32_t now) {
  Serial.print(F("mode "));
  // mode::name() returns PROGMEM strings on AVR — print flash-aware.
  Serial.print(FPSTR(mode::name(_mode)));
  Serial.print(F(" -> "));
  Serial.println(FPSTR(mode::name(next)));

  _mode = next;
  _modeStartMs = now;
}
