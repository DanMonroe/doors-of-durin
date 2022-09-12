#include <Arduino.h>

#include <DOD_PixelStrips.h>

const bool LEDS_ENABLED = false;

DOD_PixelStrips *strips = new DOD_PixelStrips();

void setup() {
  Serial.begin(9600);
  // // Serial.begin(115200);
  Serial.println("Start LED");
  Serial.println();

  if (LEDS_ENABLED) {
    strips->setupStrips();
  }

}

void loop() {
    if (LEDS_ENABLED) {
    strips->loop();
  }
}
