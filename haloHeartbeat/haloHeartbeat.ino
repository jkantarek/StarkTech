// NeoPixel ring heartbeat for the Arduino Mega (24-LED ring).
// Wiring: ring 5V -> Mega 5V, ring GND -> Mega GND, ring DIN -> Mega D6.
//   DOUT unconnected. For full reliability power the ring from an external
//   5V supply with a common ground (USB Mega rail is marginal).
// Effect: all 24 pixels breathe on a 5 s envelope at 25% peak; one pixel at
// 75% chases around. Serial diagnostics on USB at 9600.

#include <Adafruit_NeoPixel.h>

#define DATA_PIN   6
#define NUM_LEDS  24
#define AMBIENT   64    // 25% fill brightness (255*0.25)
#define CHASE    192    // 75% chase pixel (255*0.75)

Adafruit_NeoPixel ring(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

const uint32_t kPeriod = 5000;
const uint32_t kStepMs = 120;

const uint16_t PTS[][2] = {
  {0, 0}, {1200, 255}, {1650, 60}, {2000, 200}, {3000, 255}, {4100, 30}, {4250, 0}, {5000, 0}
};
const uint8_t NPTS = sizeof(PTS) / sizeof(PTS[0]);

static uint8_t envelope(unsigned long t) {
  for (uint8_t i = 1; i < NPTS; i++) {
    if (t <= PTS[i][0]) {
      long dt = PTS[i][0] - PTS[i-1][0];
      long db = PTS[i][1] - PTS[i-1][1];
      long b  = PTS[i-1][1] + (long)(t - PTS[i-1][0]) * db / dt;
      return (b < 0) ? 0 : (b > 255) ? 255 : (uint8_t)b;
    }
  }
  return 0;
}

void setup() {
  Serial.begin(9600);
  Serial.flush();
  Serial.println(F("haloHeartbeat v2 start"));

  delay(200);          // USB/UART/bootloader settle before first 800kHz frame
  ring.begin();
  ring.clear();        // all-zero frame: force every LED off first
  ring.show();
  Serial.println(F("ring cleared"));
  Serial.flush();
}

void loop() {
  unsigned long now = millis();
  uint8_t fill = (uint8_t)((uint32_t)envelope(now % kPeriod) * AMBIENT / 255);
  uint16_t chase = (now / kStepMs) % NUM_LEDS;

  ring.fill(ring.Color(fill, fill, fill));
  ring.setPixelColor(chase, ring.Color(CHASE, CHASE, CHASE));
  ring.show();

  static unsigned long lastLog = 0;
  if (now - lastLog >= 1000) {        // 1 Hz diagnostic
    lastLog = now;
    Serial.print(F("fill=")); Serial.print(fill);
    Serial.print(F(" chase=")); Serial.println(chase);
  }
}