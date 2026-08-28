#include "KnobA6.h"

#include <Arduino.h>

// A6 = 60 on the Mega (A0=54). No pinMode needed for analog input.
void KnobA6::setup() {
  _lastSampleMs = 0;
  _samples = 0;
  _raw = 0;
  _min = 1023;  // inverted: the first sample sets both bounds
  _max = 0;
}

void KnobA6::update(uint32_t now) {
  if (_sampled && now - _lastSampleMs < kSampleMs) {
    return;
  }
  _lastSampleMs = now;
  _sampled = true;

  const uint16_t v = (uint16_t)analogRead(A6);
  _raw = v;
  if (v < _min) _min = v;
  if (v > _max) _max = v;

  if (_samples < kSamples) {
    ++_samples;
  }
}

uint8_t KnobA6::percent() const {
  if (!scanned()) {
    return 0;
  }
  if (_max <= _min) {
    return 0;  // degenerate range
  }
  if (_raw <= _min) return 0;
  if (_raw >= _max) return 100;
  return (uint8_t)(((uint32_t)(_raw - _min) * 100u) / (_max - _min));
}
