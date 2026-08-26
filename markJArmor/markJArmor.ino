#include <Adafruit_NeoPixel.h>
#define PIN 6
#define N 24
Adafruit_NeoPixel ring(N, PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  ring.begin();
  ring.clear();
  Serial.println(F("start"));
}

void loop() {
  static int i = 1;                 // grow from 1..N
  Serial.print(F("set ")); Serial.println(i);
  ring.setPixelColor(i, ring.Color(3, 3, 3));  // 1%
  ring.show();
  Serial.println(F("done"));
  i++;
  if (i >= N) i = 1;               // wrap, stay minimal
  delay(1000);
}
