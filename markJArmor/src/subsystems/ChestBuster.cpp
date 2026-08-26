#include "ChestBuster.h"

#include <Arduino.h>

ChestBuster::ChestBuster() : _ring(kPixels, kPin, NEO_GRBW + NEO_KHZ800) {}

void ChestBuster::begin() {
  _ring.begin();
  _ring.clear();
  _lastStepMs = 0;
  _i = 1;  // grow from 1..kPixels, matching the original loop()
}

void ChestBuster::update(const uint32_t now) {
  if (now - _lastStepMs < kStepMs) {
    return;  // non-blocking: was delay(1000) in the old loop()
  }
  _lastStepMs = now;

  Serial.print(F("ChestBuster: set "));
  Serial.println(_i);
  _ring.setPixelColor(_i, _ring.Color(3, 3, 3));  // 1%
  _ring.show();
  Serial.println(F("ChestBuster: done"));

  _i++;
  if (_i >= kPixels) {
    _i = 1;  // wrap, stay minimal
  }
}
