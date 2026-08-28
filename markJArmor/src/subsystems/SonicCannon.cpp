#include "SonicCannon.h"

#include <Arduino.h>

// --- AVR: real Mozzi engine -------------------------------------------------
// Audio output: HIFI dual-PWM mode (MOZZI_OUTPUT_2PIN_PWM) on pins 11+12.
// pin 11 (Timer1.A) carries the high 7 bits via 3.9k, pin 12 (Timer1.B) the
// low 7 bits via 499k, summed into one node with a 4.7n cap to ground; that
// node feeds the amp's IN+ with IN- grounded (see Mozzi's HIFI examples).
// Config values must be defined BEFORE Mozzi.h is included.
#ifdef __AVR__
#include <MozziConfigValues.h>
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>

namespace {

Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aCannon(SIN2048_DATA);

// Sampled by updateAudio() every audio frame; set by Machine each tick.
volatile bool g_active = false;

}  // namespace

void updateControl() {
  // Constant tone — nothing to sweep at control rate.
}

AudioOutput updateAudio() {
  // aCannon.next() (int8 sine table) centered at 0, directly as 16-bit
  // (Mozzi scales to the 14-bit 2-pin DAC); silence when inactive. No
  // from8Bit — that would halve the level.
  return MonoOutput::from16Bit(g_active ? (int16_t)aCannon.next() : 0);
}

#endif  // __AVR__

void SonicCannon::setup() {
#ifdef __AVR__
  Serial.println(F("sonic: startMozzi (hifi pins 11+12)"));
  startMozzi(64);  // control rate 64 Hz; the tone is constant
  aCannon.setFreq(440);
#else
  Serial.println(F("sonic: stub (host build)"));
#endif
}

void SonicCannon::update(const uint32_t now, bool active) {
  (void)now;
  _active = active;
#ifdef __AVR__
  g_active = active;
  audioHook();  // fill the audio buffer (keeps PWM live in both states)
#endif
}

const char* SonicCannon::status() const {
  static const char kOff[] PROGMEM = "off";
  static const char kFire[] PROGMEM = "fire";
  return _active ? kFire : kOff;
}