#include "HeadsUpDisplay.h"

#include <Arduino.h>

// String constants the HUD deals with (tags/statuses) live in flash on AVR,
// in RAM on the host. Render them through PGM-cleared reads so the byte
// emitted is correct on both targets — a raw `*s` of a flash address crashes
// the Mega (the PROGMEM-as-RAM bug).
#ifdef __AVR__
#include <avr/pgmspace.h>
#define HUD_CHAR(p) ((char)pgm_read_byte(&(p)))
#else
#define HUD_CHAR(p) (p)  // host: literals are RAM-backed
#endif

namespace {

// Compare two literal strings (flash on AVR, RAM on host) for equality.
bool sameString(const char* a, const char* b) {
  char ca;
  do {
    ca = HUD_CHAR(*a);
    if (ca != HUD_CHAR(*b)) {
      return false;
    }
    ++a;
    ++b;
  } while (ca != '\0');
  return true;
}

// Copy `src` (flash on AVR, RAM on host) into `dst` up to `dstCap-1` bytes.
// Returns the byte count written (the NUL is not counted).
size_t copyString(char* dst, size_t dstCap, const char* src) {
  size_t n = 0;
  char c;
  while (n + 1 < dstCap && (c = HUD_CHAR(*src)) != '\0') {
    dst[n++] = c;
    ++src;
  }
  dst[n] = '\0';
  return n;
}

}  // namespace

void HeadsUpDisplay::report(const char* tag, const char* status) {
  if (status == nullptr) {
    return;  // subsystem has nothing to report this cycle
  }
  for (uint16_t i = 0; i < _count; ++i) {
    if (sameString(_entries[i].tag, tag)) {
      _entries[i].status = status;  // latest status wins
      return;
    }
  }
  if (_count < kMaxEntries) {
    _entries[_count].tag = tag;
    _entries[_count].status = status;
    ++_count;
  }
}

void HeadsUpDisplay::update(const uint32_t now) {
  static const char kSep[] PROGMEM = ".status - ";  // flash literal (HUD_CHAR)
  if (_logged && now - _lastLogMs < kLogMs) {
    return;  // not a second boundary yet
  }
  _lastLogMs = now;
  _logged = true;

  for (uint16_t i = 0; i < _count; ++i) {
    size_t off = 0;
    off += copyString(_line + off, sizeof(_line) - off, _entries[i].tag);
    off += copyString(_line + off, sizeof(_line) - off, kSep);
    off += copyString(_line + off, sizeof(_line) - off, _entries[i].status);
    Serial.println(_line);
    Serial.flush();  // drain TX before the next ring show()
  }
  _count = 0;  // entries are replaced next cycle, not appended forever
}