#pragma once
// Minimal Arduino seam for host tests — the ONLY Arduino mock the suite
// needs. Pure-logic modules (Mode, InputToggles, SonicCannon) never see it;
// Machine.cpp / ChestBuster.cpp / KnobA6.cpp pull in <Arduino.h> and use
// F(), millis(), Serial and analogRead().
// NEVER compiled for the board: arduino-cli compiles only the sketch + src/,
// so test/ cannot leak into firmware.
#include <stdint.h>
#include <string>

#define F(x) (x)  // Arduino PROGMEM wrapper; plain literal on host
#define A6 (54)   // Mega analog pin 6 (A0 = 54)

// Flash-string qualifiers: no-ops on the host — literals are RAM-backed, and
// HUD's HUD_CHAR() reads them with plain dereferences (see its __AVR__ guard).
#define PROGMEM
#define PSTR(x) (x)
#define FPSTR(x) (x)

// Deterministic clock: every TU calls test_now_ms() wherever firmware calls
// millis(), so the AVR Timer0 timebase is faithfully replaced by the test.
#define millis() test_now_ms()
unsigned long test_now_ms();

// Scripted ADC: the sketch calls analogRead() where it wants a sample; each
// test provides test_analog_read() (KnobA6 tests script values per call).
int test_analog_read(uint8_t pin);
inline int analogRead(uint8_t pin) { return test_analog_read(pin); }

class SerialClass {
 public:
  void begin(unsigned long) {}
  // Recording seam for HUD tests: const char*/char* overloads append to
  // log; numeric prints are discarded. The char* overloads matter: firmware
  // render buffers are char[N] and would otherwise bind the discarding
  // template (array-to-pointer beats the const-qualified overload).
  void print(const char* s) { logBuf() += s; }
  void println(const char* s) {
    logBuf() += s;
    logBuf() += '\n';
  }
  void print(char* s) { logBuf() += s; }
  void println(char* s) {
    logBuf() += s;
    logBuf() += '\n';
  }
  template <typename T>
  void print(T) {}
  template <typename T>
  void println(T) {}
  void flush() {}

  static const std::string& log() { return logBuf(); }
  static void clear() { logBuf().clear(); }

 private:
  // Function-local static: header-only definition, one instance per program
  // (no out-of-line member definition needed).
  static std::string& logBuf() {
    static std::string s;
    return s;
  }
};
// static + attribute: each TU has its own instance; TUs that only compile a
// subsystem (which uses it inside its .cpp) still link without warnings.
static SerialClass Serial __attribute__((unused));

inline void delay(unsigned long) {}