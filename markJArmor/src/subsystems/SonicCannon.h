#pragma once

// Sound subsystem: the simplest Mozzi playback — ONE 8-bit repulsor blast
// (8 kHz PROGMEM table) fired once at boot, played through once, no looping,
// no mode gating. See SonicCannon.cpp for the exact Mozzi seam.
class SonicCannon {
 public:
  // startMozzi + fire the blast once. Call once from Machine::setup.
  void setup();

  // Pump the audio buffer. Call every machine tick.
  void update();

  // HUD status: "fire" while the startup sound plays, "off" after it ends.
  const char* status() const;
};