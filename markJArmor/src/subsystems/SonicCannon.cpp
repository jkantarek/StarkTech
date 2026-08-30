#include "SonicCannon.h"

#include <Arduino.h>

// --- AVR: real Mozzi engine -------------------------------------------------
// Audio output: single-pin PWM mode (MOZZI_OUTPUT_PWM) on pin 11 (Timer1.A),
// Mozzi's default. Config values must be defined BEFORE Mozzi.h.
#ifdef __AVR__
#include <MozziConfigValues.h>
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM
#include <Mozzi.h>
#include <Sample.h>

// ONE sample compiled in (the first blast). The other RepulsorBlast2..7.h
// tables stay in src/subsystems/SonicCannon/ — swap this include (and the
// array/count names below) to upload a different blast.
#include "SonicCannon/RepulsorBlast1.h"

namespace {

// One-shot sample of the 8 kHz blast.
Sample<K_REPULSOR_BLAST_1_SAMPLES, AUDIO_RATE> aBlast(kRepulsorBlast1Samples);

}  // namespace

void updateControl() {
  // Nothing to sweep — one-shot sample.
}

AudioOutput updateAudio() {
  // One audio frame per call: the blast table replayed at natural speed
  // (see setup); next() returns 0 once the table is exhausted (one-shot).
  // from8Bit passes through in 8-bit PWM mode — full ±128 range on pin 11.
  return MonoOutput::from8Bit((int16_t)aBlast.next());
}

#endif  // __AVR__

void SonicCannon::setup() {
#ifdef __AVR__
  Serial.println(F("sonic: startMozzi (PWM pin 11)"));
  startMozzi();  // control rate 64 Hz
  aBlast.setLoopingOff();  // one-shot, no loop
  // Natural speed: Sample::setFreq(f) plays the whole table f times/sec, so
  // f = table's sample rate / table length replays at recorded tempo (Mozzi
  // Samples example: SR/NUM_CELLS).
  aBlast.setFreq((float)8000.0f / (float)K_REPULSOR_BLAST_1_SAMPLES);
  aBlast.start();  // FIRE once at boot
#else
  Serial.println(F("sonic: stub (host build)"));
#endif
}

void SonicCannon::update() {
#ifdef __AVR__
  audioHook();  // fill the audio buffer (required every loop)
#endif
}

const char* SonicCannon::status() const {
#ifdef __AVR__
  static const char kFire[] PROGMEM = "fire";
  static const char kOff[] PROGMEM = "off";
  return aBlast.isPlaying() ? kFire : kOff;
#else
  return nullptr;  // host build: nothing reported
#endif
}
