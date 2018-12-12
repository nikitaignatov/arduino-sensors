#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN            12
#define NUMPIXELS      24
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);

int max_brightness = 150;
int pixel_delay = 8;
int cycle_delay = 0;
int delayval = 50; // delay for half a second

void setup() {
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
  
  turn_on();
  delay(10 * 1000);
  turn_off();
  pixels.clear();
  delay(3 * 1000);
  
}

void loop() {
}

void turn_on() {
  for (int b = 1; b < max_brightness; b++) {
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, b, max(0, b / 3), 0);
      pixels.show();
      delay(pixel_delay);
    }
    delay(cycle_delay);
  }
}

void turn_off() {
  for (int b = max_brightness; b >= 0 ; b--) {
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, b, max(0, b / 3), 0);
      pixels.show();
      delay(pixel_delay);
    }
    delay(cycle_delay);
  }
}
