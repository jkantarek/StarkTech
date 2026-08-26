// markJArmor — Iron Man armor firmware.
//
// The sketch is now a thin entry point: hardware work lives in src/subsystems
// (ChestBuster owns the NeoPixel ring, SonicCannon/InputToggles are stubbed)
// and the run loop is a millis()-based state machine in src/state (Mode +
// Machine). No blocking delay() calls remain.
#include "src/state/Machine.h"

Machine machine;

void setup() {
  machine.setup();
}

void loop() {
  machine.update();
}
