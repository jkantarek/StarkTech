#pragma once
// Minimal Arduino seam for host tests — the ONLY Arduino mock the suite
// needs. Pure-logic modules (Mode, InputToggles, SonicCannon) never see it;
// only Machine.cpp / ChestBuster.cpp pull in <Arduino.h> and they use just
// F(), millis() and Serial.
// NEVER compiled for the board: arduino-cli compiles only the sketch + src/,
// so test/ cannot leak into firmware.
#include <stdint.h>

#define F(x) (x)  // Arduino PROGMEM wrapper; plain literal on host

// Deterministic clock: every TU calls test_now_ms() wherever firmware calls
// millis(), so the AVR Timer0 timebase is faithfully replaced by the test.
#define millis() test_now_ms()
unsigned long test_now_ms();

class SerialClass {
 public:
  void begin(unsigned long) {}
  template <typename T>
  void print(T) {}
  template <typename T>
  void println(T) {}
  void flush() {}
};
static SerialClass Serial;

inline void delay(unsigned long) {}