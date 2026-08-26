#include "Machine.h"

#include <Arduino.h>

void Machine::setup() {
  Serial.begin(9600);
  Serial.println(F("start"));

  _chest.begin();
  _sonic.setup();

  _mode = Mode::STANDBY;
  _modeStartMs = millis();
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