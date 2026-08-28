#include "HeadsUpDisplay.h"

#include <Arduino.h>

// String constants the HUD deals with (subsystem name()/tag/keys) live in
// flash on AVR, in RAM on the host. Render them through PGM-cleared reads so
// the byte emitted is correct on both targets — a raw `*s` of a flash
// address crashes the Mega (the PROGMEM-as-RAM bug).
#ifdef __AVR__
#include <avr/pgmspace.h>
#define HUD_CHAR(p) ((char)pgm_read_byte(&(p)))
#else
#define HUD_CHAR(p) (p)  // host: F(x)=x, literals are RAM-backed
#endif

namespace {

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

void HeadsUpDisplay::attach(const char* tag, Subsystem* subsystem) {
  return;  // STUB: no-op while the HUD is disabled (see HeadsUpDisplay.h)
  if (_count >= kMaxEntries) {
    return;  // full: keep the oldest entries, drop the newcomer
  }
  _entries[_count].tag = tag;
  _entries[_count].subsystem = subsystem;
  ++_count;
}

void HeadsUpDisplay::update(const uint32_t now) {
  return;  // STUB: no-op while the HUD is disabled (see HeadsUpDisplay.h)
  if (_logged && now - _lastLogMs < kLogMs) {
    return;  // not a second boundary yet
  }
  _lastLogMs = now;
  _logged = true;

  for (uint16_t i = 0; i < _count; ++i) {
    EntrySink sink(_line, sizeof(_line));
    // Prefix "[HUD] <tag>: " — tag is flash on AVR, so copy it PROGMEM-safe.
    size_t off = 0;
    off += copyString(_line + off, sizeof(_line) - off, "[HUD] ");
    off += copyString(_line + off, sizeof(_line) - off, _entries[i].tag);
    off += copyString(_line + off, sizeof(_line) - off, ": ");
    sink.seek(off);  // continue appending subsystem fields after the prefix

    _entries[i].subsystem->status(sink);
    Serial.println(_line);
    Serial.flush();  // drain TX before the next ring show()
  }
}

// --- EntrySink: field accumulation into the caller's buffer ---

void HeadsUpDisplay::EntrySink::seek(size_t pos) {
  _len = pos;  // already at \0 (copy_string terminated)
  _fields = 0;
}

void HeadsUpDisplay::EntrySink::field(const char* name, uint32_t value) {
  if (_fields++) {
    put(" ");
  }
  put(name);
  put("=");
  putDec(value);
}

void HeadsUpDisplay::EntrySink::field(const char* name, const char* value) {
  if (_fields++) {
    put(" ");
  }
  put(name);
  put("=");
  put(value);
}

void HeadsUpDisplay::EntrySink::put(const char* s) {
  char c;
  while (_len + 1 < _cap && (c = HUD_CHAR(*s)) != '\0') {
    _buf[_len++] = c;
    ++s;
  }
  _buf[_len] = '\0';
}

void HeadsUpDisplay::EntrySink::putDec(uint32_t v) {
  char digits[10];
  uint8_t n = 0;
  do {
    digits[n++] = (char)('0' + (v % 10));
    v /= 10;
  } while (v && n < sizeof(digits));
  while (n && _len + 1 < _cap) {
    _buf[_len++] = digits[--n];
  }
  _buf[_len] = '\0';
}
