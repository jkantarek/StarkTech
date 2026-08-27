#include "ChestBuster.h"

#include <Arduino.h>

ChestBuster::ChestBuster() : _ring(kPixels, kPin, NEO_GRBW + NEO_KHZ800) {}

void ChestBuster::setup(cb::Animation* animation) {
  _ring.begin();
  Serial.println(F("ring.begin - OK"));
  // Dim the strip to ~4% — the original that NEVER crashed ran 1% (3,3,3) at
  // 1 s. The fast gradient build runs full-saturation colors at 100 frames/s:
  // bright strip I/O on USB 5V shared with the Mega is the prime reset suspect
  // (every Adafruit example calls setBrightness() — ours never did). One-line
  // test: if the board stops resetting with this, brightness/current was it.
  _ring.setBrightness(50);
  Serial.println(F("chest: brightness 50"));
  //_ring.show();
  Serial.println(F("ring.boot - SKIPPED (bisect)"));
  _lastStepMs = 0;
  _animation = animation;
  _animation->begin(kPixels);
  Serial.println(F("chest: ring ok (24 px @ pin 6, off)"));
}

void ChestBuster::update(const uint32_t now) {
  if (now - _lastStepMs < kStepMs) {
    return;  // gate miss — non-blocking core; silent
  }

  // Step-timer reset — visible on serial per the bisect request. now is
  // millis(); delta (now - last) is the actual elapsed time, always >= 1000.
  _lastStepMs = now;

  if(_animation->step(_frame) > 0) {
  Serial.print(F("ChestBuster.set "));
  Serial.println(_animation->step(_frame));  // advance + fill the frame
  }
  for (uint16_t i = 0; i < kPixels; ++i) {
    _ring.setPixelColor(i, _frame[i]);  // 4% brightness via setBrightness
  }
  _ring.show();
}
