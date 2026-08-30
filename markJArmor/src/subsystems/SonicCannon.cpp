#include "SonicCannon.h"

#include <Arduino.h>

// --- AVR: real Mozzi engine -------------------------------------------------
// Audio output: single-pin PWM mode (MOZZI_OUTPUT_PWM) on pin 11 (Timer1.A),
// Mozzi's default. Config values must be defined BEFORE Mozzi.h.
// Pin 11 feeds the Adafruit STEMMA Audio Amp (PAM8302, PID 5647) through its
// JST-PH 3-pin connector (SIG/5V/GND; input referenced to signal ground).
// That board has no mute pin — SD/shutdown is pulled up on-board, so just a
// signal-in and a common ground.
#ifdef __AVR__
#include <MozziConfigValues.h>
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM
#include <Mozzi.h>

// 16-bit fidelity test: play the source-resolution table (extracted from the
// original mkv audio, int16_t) instead of the 8-bit cut. Mozzi's Sample<>
// only accepts int8_t tables (ctor takes `const int8_t *`), so the table is
// read directly with a Q16.16 phase and handed to MonoOutput::from16Bit() —
// just fromNBit(16, ...), the full -32768..32767 range on the PWM pin.
//
// Revert to the 8-bit Sample path:
//   #include <Sample.h>
//   #include "SonicCannon/RepulsorBlast1.h"
//   Sample<K_REPULSOR_BLAST_1_SAMPLES, AUDIO_RATE> aBlast(kRepulsorBlast1Samples);
//     // setup: aBlast.setLoopingOff(); aBlast.setFreq(8000.0f / K_REPULSOR_BLAST_1_SAMPLES); aBlast.start();
//     // updateAudio: return MonoOutput::from8Bit((int16_t)aBlast.next());
//     // status(): aBlast.isPlaying()
#include "SonicCannon/RepulsorBlast1-16b.h"

namespace {

// One-shot player for the 16-bit blast table. Phase is Q16.16: each audio
// frame advances 8000/16384 of a table sample, so the 27040-sample table
// replays in 27040/8000 = 3.38 s at natural pitch.
constexpr uint32_t kPhaseStep =
    (8000UL << 16) / static_cast<uint32_t>(AUDIO_RATE);  // 32000 @ 16384
uint32_t gPhase = 0u;
bool gPlaying = false;

// Table is split into two halves: AVR's 16-bit sizeof caps a flat array at
// 32767 bytes, and 27040 int16 = 54080 B does not fit.
inline int16_t readSample(uint32_t i) {
#ifdef __AVR__
  return i < K_REPULSOR_BLAST_1_16B_SAMPLES / 2
      ? static_cast<int16_t>(pgm_read_word_near(&kRepulsorBlast1_16bSamplesPart1[i]))
      : static_cast<int16_t>(pgm_read_word_near(
            &kRepulsorBlast1_16bSamplesPart2[i - K_REPULSOR_BLAST_1_16B_SAMPLES / 2]));
#else
  return i < K_REPULSOR_BLAST_1_16B_SAMPLES / 2
      ? kRepulsorBlast1_16bSamplesPart1[i]
      : kRepulsorBlast1_16bSamplesPart2[i - K_REPULSOR_BLAST_1_16B_SAMPLES / 2];
#endif
}

void startBlast() {
  gPhase = 0u;
  gPlaying = true;
}

}  // namespace

void updateControl() {
  // Nothing to sweep — one-shot sample.
}

AudioOutput updateAudio() {
  int16_t sample = 0;
  if (gPlaying) {
    const uint32_t idx = gPhase >> 16;
    if (idx >= K_REPULSOR_BLAST_1_16B_SAMPLES) {
      gPlaying = false;  // one-shot: table exhausted
    } else {
      sample = readSample(idx);
    }
    gPhase += kPhaseStep;
  }
  return MonoOutput::from16Bit(sample);
}

#endif  // __AVR__

void SonicCannon::setup() {
#ifdef __AVR__
  Serial.println(F("sonic: startMozzi (PWM pin 11)"));
  startMozzi();  // control rate 64 Hz
  startBlast();  // FIRE once at boot
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
  return gPlaying ? kFire : kOff;
#else
  return nullptr;  // host build: nothing reported
#endif
}
