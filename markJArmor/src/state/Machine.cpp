#include "Machine.h"

#include <Arduino.h>

void Machine::setup() {
  Serial.begin(9600);
  Serial.println(F("start"));

  //_input.setup();  // prints its own progress
  _chest.setup(&_loader);  // prints its own progress; attach the loader
  //_sonic.setup();  // prints its own progress

  _mode = Mode::ACTIVATED;
  _modeStartMs = millis();
  Serial.println(F("machine: ready"));

  // Drain our TX, then give the host a quiet window to receive the full boot
  // block before update() starts. Setup-only pause (never in the loop): if a
  // reset-cut happens later, every boot line still arrives intact so the log
  // bisects setup() vs update() cleanly.
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
      _sonic.update(now);  // no-op until sound hardware exists
      break;

    case Mode::STANDBY:
    default:
      // Nothing active while resting: subsystems are paused, not stopped.
      break;
  }
}

Mode Machine::mode() const {
  return _mode;
}

void Machine::enterMode(const Mode next, const uint32_t now) {
  Serial.print(F("mode "));
  Serial.print(mode::name(_mode));
  Serial.print(F(" -> "));
  Serial.println(mode::name(next));

  _mode = next;
  _modeStartMs = now;
}
