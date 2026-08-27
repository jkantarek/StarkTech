#include "ChestBuster.h"

#include <Arduino.h>

ChestBuster::ChestBuster() : _ring(kPixels, kPin, NEO_GRBW + NEO_KHZ800) {}

void ChestBuster::setup() {
  _ring.begin();
  Serial.println(F("ring.begin - OK"));
  // BISECT: boot show() REMOVED — original-parity test. The original sketch
  // had NO boot transmission (begin + clear only, first show in loop).
  // Matrix so far: boot-show only => passes; boot+update shows => crashes;
  // update-show only => UNTESTED (this). If this passes, the crash needs
  // BOTH a boot frame AND loop frames; if it still crashes, boot show is
  // exonerated and the next split is blocking vs hot-gate timing.
  //_ring.show();
  Serial.println(F("ring.boot - SKIPPED (bisect)"));
  _lastStepMs = 0;
  _i = 0;  // LED index 0 = LED #1; pass covers all kPixels LEDs
  Serial.println(F("chest: ring ok (24 px @ pin 6, off)"));
}

void ChestBuster::update(const uint32_t now) {
  if (now - _lastStepMs < kStepMs) {
    return;  // gate miss — non-blocking core; silent
  }

  // Step-timer reset — visible on serial per the bisect request. now is
  // millis(); delta (now - last) is the actual elapsed time, always >= 1000.
  Serial.print(F("tick t="));
  Serial.print(now);
  Serial.print(F(" last="));
  Serial.print(_lastStepMs);
  Serial.print(F(" delta="));
  Serial.println(now - _lastStepMs);
  _lastStepMs = now;

  // Step prints restored: the original sketch that WORKED had Serial around
  // the strip ops; the print-free build crashed. Keep original parity here.
  Serial.print(F("set "));
  Serial.println(_i);
  _ring.setPixelColor(_i, _ring.Color(0, 0+_i*10, 255-(_i*10)));  // 1%
  _ring.show();
  Serial.println(F("done"));

  _i++;  // uint16_t, bounded by the wrap check below — cannot overflow.
  if (_i >= kPixels) {
    // Wrap check fires at _i == 24. `_i` here is the value AFTER the show
    // and increment — pixel 23 was the last lit, and we reset before the
    // next tick so the pass restarts at index 0.
    Serial.print(F("wrap: i was "));
    Serial.print(_i);
    Serial.println(F(" -> reset to 0 (restart)"));
    _i = 0;
  }
}
