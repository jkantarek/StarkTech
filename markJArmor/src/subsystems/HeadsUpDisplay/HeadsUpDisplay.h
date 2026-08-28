#pragma once

#include <stddef.h>
#include <stdint.h>

// Generic 1 Hz stats logger. Subsystems implement no interface and nothing
// is attached by pointer: each subsystem exposes a simple
// `const char* status()` that returns either nullptr (nothing to report) or
// a short PROGMEM string, and Machine reports it here under a tag.
// report() keeps only the latest status per tag; update() renders
// "<submodule>.status - <message>" (e.g. "chest.status - sweep") for every
// reported tag once per second.
//
// Tags/statuses are PROGMEM strings on AVR; every char is read
// PROGMEM-safely (HUD_CHAR) so lines build correctly on the host too (where
// literals are RAM-backed). Lines are assembled in a fixed char buffer;
// only the final Serial.println touches the serial seam.
class HeadsUpDisplay {
 public:
  // Record `status` for `tag`. nullptr skips the tag this cycle; a repeated
  // tag replaces its previous status (subsystem reports every tick, so
  // entries are refreshed, never appended).
  void report(const char* tag, const char* status);

  // Render all reported lines if >= kLogMs elapsed since the last render
  // (logs immediately on the first call). Call every machine tick.
  void update(const uint32_t now);

 private:
  struct Entry {
    const char* tag;
    const char* status;
  };

  static constexpr uint32_t kLogMs = 1000;   // one render per second
  static constexpr uint16_t kMaxEntries = 8; // distinct tag cap

  Entry _entries[kMaxEntries];
  uint16_t _count = 0;
  uint32_t _lastLogMs = 0;
  bool _logged = false;  // first update() always logs (boot t may be 0)

  char _line[64];  // per-entry render buffer (reused across entries)
};