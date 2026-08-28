#include "ChestBuster.h"

#include <Arduino.h>

ChestBuster::ChestBuster() : _ring(kPixels, kPin, NEO_GRBW + NEO_KHZ800) {}

void ChestBuster::setup(cb::Animation* animation) {
  _ring.begin();
  Serial.println(F("ring.begin - OK"));
  // Limit current draw: 50/255 (~20%) of full brightness — the strip shares
  // the USB 5V rail with the Mega.
  _ring.setBrightness(50);
  Serial.println(F("chest: brightness 50"));
  Serial.println(F("ring.boot - SKIPPED (bisect)"));
  _lastStepMs = 0;
  _animation = animation;
  _animation->begin(kPixels);
  Serial.println(F("chest: ring ok (24 px @ pin 6, off)"));
}

void ChestBuster::update(const uint32_t now) {
  if (now - _lastStepMs < kStepMs) {
    return;  // step period not elapsed yet — this tick does nothing
  }

  // Step timer reset; delta (now - last) is the elapsed time (>= kStepMs on
  // entry). now is millis().
  _lastStepMs = now;

  if(_animation->step(_frame) > 0) {
  Serial.print(F("ChestBuster.set "));
  Serial.println(_animation->step(_frame));  // advance + fill the frame
  }
  for (uint16_t i = 0; i < kPixels; ++i) {
    _ring.setPixelColor(i, _frame[i]);  // brightness capped via setBrightness
  }
  _ring.show();
}
