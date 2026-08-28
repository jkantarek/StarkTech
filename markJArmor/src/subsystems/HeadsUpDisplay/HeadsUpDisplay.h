#pragma once

#include <stddef.h>
#include <stdint.h>

// Reporting contract for the HeadsUpDisplay. A subsystem that wants HUD lines
// implements Subsystem (just status()); the HUD learns nothing concrete about
// it. Field emission goes through a StatusSink so the renderer stays dumb.
class StatusSink {
 public:
  virtual ~StatusSink() {}

  // Emit one "name=value" field. Name is a short key (e.g. "raw", "pct"),
  // value is either a number or a short string (e.g. an animation name).
  virtual void field(const char* name, uint32_t value) = 0;
  virtual void field(const char* name, const char* value) = 0;
};

class Subsystem {
 public:
  virtual ~Subsystem() {}

  // Emit this subsystem's current stats through `out`. Pure query — must not
  // mutate state, sample hardware, or block. Called by the HUD once a second.
  virtual void status(StatusSink& out) const = 0;
};

// Generic 1 Hz stats logger. STUBBED: attach() and update() are no-ops, so
// nothing is logged or rendered; the interface below is the intended contract
// and is being reworked — subsystems will expose a simple current status
// string instead of this sink seam.
//
// When active, lines are assembled in a fixed char buffer (no dependency on
// SerialClass, so this header — and the sink it builds — compiles the same on
// AVR and the host mock); only the final Serial.println(line) touches the
// serial seam.
class HeadsUpDisplay {
 public:
  // Register a subsystem under `tag` for periodic logging. Bounded: once
  // kMaxEntries are attached, callers are ignored (oldest entries kept).
  void attach(const char* tag, Subsystem* subsystem);

  // Render all attached lines if >= kLogMs elapsed since the last render
  // (logs immediately on the first call). Call every machine tick.
  void update(const uint32_t now);

 private:
  // Accumulates "name=value" fields into a caller-owned char buffer.
  class EntrySink : public StatusSink {
   public:
    EntrySink(char* buf, size_t cap) : _buf(buf), _cap(cap) {}
    void field(const char* name, uint32_t value) override;
    void field(const char* name, const char* value) override;

    // Continue appending at a previously-written offset (the "[HUD] tag: "
    // prefix). Public so HeadsUpDisplay::update can seed the line, then the
    // subsystem's status() fields follow.
    void seek(size_t pos);

   private:
    void put(const char* s);
    void putDec(uint32_t v);

    char* _buf;
    size_t _cap;
    size_t _len = 0;
    uint8_t _fields = 0;
  };

  static constexpr uint32_t kLogMs = 1000;   // one render per second
  static constexpr uint16_t kMaxEntries = 8; // subsystem slot cap

  struct Entry {
    const char* tag;
    Subsystem* subsystem;
  };

  Entry _entries[kMaxEntries];
  uint16_t _count = 0;
  uint32_t _lastLogMs = 0;
  bool _logged = false;  // first update() always logs (boot t may be 0)

  char _line[64];  // per-entry render buffer (reused across entries)
};