#pragma once

#include <stdint.h>

// Analog knob on A6 — samples on a fixed cadence, tracks observed min/max,
// and maps the current reading to 0..100. Pure logic + one analogRead(A6);
// no virtual dispatch, no flash-string handling.
class KnobA6 {
 public:
  void setup();
  void update(uint32_t now);

  bool scanned() const { return _samples >= kSamples; }
  uint16_t raw() const { return _raw; }
  uint16_t minRaw() const { return _min; }
  uint16_t maxRaw() const { return _max; }

  // Normalized position in [min,max] -> 0..100; 0 while unscanned.
  uint8_t percent() const;

  // Clear scan state and re-arm the boot sweep (bounds re-derive from the
  // next samples).
  void rescan();

  // HUD status: nullptr until the boot scan completes, then "ready"
  // (flash literal).
  const char* status() const { return scanned() ? "ready" : nullptr; }

 private:
  static constexpr uint16_t kSampleMs = 20;
  static constexpr uint16_t kSamples = 50;

  uint32_t _lastSampleMs = 0;
  bool _sampled = false;   // first update() always samples
  uint16_t _samples = 0;
  uint16_t _raw = 0;
  uint16_t _min = 1023;    // inverted: first sample sets both bounds
  uint16_t _max = 0;
};