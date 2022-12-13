#include "DOD_PixelStrips.h"
#include "DOD_Pixel.h"

#include <Arduino.h>

#define NUM_LEDS_DOOR_LEFT 287
#define NUM_LEDS_DOOR_RIGHT 276
#define DATA_PIN_DOOR_LEFT A0
#define DATA_PIN_DOOR_RIGHT A1

#define NUM_LEDS_MONITOR_LEFT 84
#define NUM_LEDS_SYMBOL_LEFT 20
#define NUM_LEDS_INNER_LEFT 104

#define NUM_LEDS_MONITOR_RIGHT 84
#define NUM_LEDS_SYMBOL_RIGHT 20
#define NUM_LEDS_INNER_RIGHT 104

#define DATA_PIN_INNER_LEFT A2
#define DATA_PIN_INNER_RIGHT A3

#define DATA_PIN_HUE_MONITOR A7
#define DATA_PIN_BRIGHTNESS_MONITOR A6
// #define DATA_PIN_HUE_MONITOR A6
// #define DATA_PIN_BRIGHTNESS_MONITOR A7

// const int NUM_LEDS_TOTAL = NUM_LEDS_DOOR_LEFT + NUM_LEDS_DOOR_RIGHT;

uint8_t MASTER_BRIGHTNESS = 200;

uint8_t min_brightness = 30;   // Set a minimum brightness level.
uint8_t brightness;

int potVal_Hue;         // Variable to store potentiometer A value (for hue)
int potVal_Brightness;  // Variable to store potentiometer B value (for brightness)
uint8_t hue_monitor;            // Hue color (0-255)

int last_hue = -1;

// https://github.com/FastLED/FastLED/blob/master/examples/Pacifica/Pacifica.ino
CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };



CRGB ledsLeftDoor[NUM_LEDS_DOOR_LEFT];
CRGB ledsRightDoor[NUM_LEDS_DOOR_RIGHT];

CRGB ledsInnerLeft[NUM_LEDS_INNER_LEFT];
CRGB ledsInnerRight[NUM_LEDS_INNER_RIGHT];
// CRGB ledsLeftMonitor[NUM_LEDS_MONITOR_LEFT];
// CRGB ledsLeftSymbol[NUM_LEDS_SYMBOL_LEFT];
// CRGB ledsRightMonitor[NUM_LEDS_MONITOR_RIGHT];
// CRGB ledsRightSymbol[NUM_LEDS_SYMBOL_RIGHT];

void DOD_PixelStrips::setupStrips() {
  Serial.println("setupStrips");

  // limit my draw to 20A at 5v of power draw
  FastLED.setMaxPowerInVoltsAndMilliamps(5,20000); 

  FastLED.setBrightness( MASTER_BRIGHTNESS );

  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_LEFT, GRB>(ledsLeftDoor, NUM_LEDS_DOOR_LEFT).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_RIGHT, GRB>(ledsRightDoor, NUM_LEDS_DOOR_RIGHT).setCorrection( TypicalLEDStrip );

  FastLED.addLeds<WS2812B, DATA_PIN_INNER_LEFT, GRB>(ledsInnerLeft, NUM_LEDS_INNER_LEFT).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812B, DATA_PIN_INNER_RIGHT, GRB>(ledsInnerRight, NUM_LEDS_INNER_RIGHT).setCorrection( TypicalLEDStrip );

  // FastLED.addLeds<WS2812B, DATA_PIN_INNER_LEFT, GRB>(ledsLeftMonitor, NUM_LEDS_MONITOR_LEFT).setCorrection( TypicalLEDStrip );
  // FastLED.addLeds<WS2812B, DATA_PIN_INNER_LEFT, GRB>(ledsLeftSymbol, NUM_LEDS_MONITOR_LEFT + NUM_LEDS_SYMBOL_LEFT).setCorrection( TypicalLEDStrip );

  // FastLED.addLeds<WS2812B, DATA_PIN_INNER_RIGHT, GRB>(ledsRightMonitor, NUM_LEDS_MONITOR_RIGHT).setCorrection( TypicalLEDStrip );
  // FastLED.addLeds<WS2812B, DATA_PIN_INNER_RIGHT, GRB>(ledsRightSymbol, NUM_LEDS_MONITOR_RIGHT + NUM_LEDS_SYMBOL_RIGHT).setCorrection( TypicalLEDStrip );

  fill_solid(ledsLeftDoor, NUM_LEDS_DOOR_LEFT, CRGB::Black);
  fill_solid(ledsRightDoor, NUM_LEDS_DOOR_RIGHT, CRGB::Black);
  
  fill_solid(ledsInnerLeft, NUM_LEDS_INNER_LEFT, CRGB::LightSkyBlue);
  fill_solid(ledsInnerRight, NUM_LEDS_INNER_RIGHT, CRGB::Black);
  // fill_solid(ledsLeftMonitor, NUM_LEDS_MONITOR_LEFT, CRGB::Black);
  // fill_solid(ledsLeftSymbol, NUM_LEDS_SYMBOL_LEFT, CRGB::Black);

  // fill_solid(ledsRightMonitor, NUM_LEDS_MONITOR_RIGHT, CRGB::Black);
  // fill_solid(ledsRightSymbol, NUM_LEDS_SYMBOL_RIGHT, CRGB::Black);

  // pinMode(DATA_PIN_HUE_MONITOR, INPUT);         // Set pin as an input.
  // pinMode(DATA_PIN_BRIGHTNESS_MONITOR, INPUT);  // Set pin as an input.
  
  FastLED.show();
// Serial.print("yo");
}

void DOD_PixelStrips::loop() {
  EVERY_N_MILLISECONDS(20) {
  // EVERY_N_MILLISECONDS(20) {
    // DOD_PixelStrips::pacifica_loop();
    // DOD_PixelStrips::checkKnobs();
    // fill_solid(ledsInnerLeft, NUM_LEDS_MONITOR_LEFT, CRGB::OrangeRed);


  int my_potVal_Hue = analogRead(DATA_PIN_HUE_MONITOR);  // Read potentiometer A (for hue).

  CHSV spectrumcolor;
  spectrumcolor.saturation = 	255;
  spectrumcolor.value = 255;

  if(my_potVal_Hue != last_hue) {
    int my_hue_monitor = map(my_potVal_Hue, 0, 1023, 0, 255);  // map(value, fromLow, fromHigh, toLow, toHigh)
    spectrumcolor.hue = my_hue_monitor;
    last_hue = my_potVal_Hue;
  // } else {
  //   spectrumcolor.hue = random(255);
  //   last_hue = -1;
  }
  for (int i = 0; i < NUM_LEDS_MONITOR_LEFT; i++) {
    hsv2rgb_spectrum( spectrumcolor, ledsInnerLeft[i] );
    // ledsInnerLeft[i].setHue(random_color);
  }

  // int my_potVal_Brightness = analogRead(DATA_PIN_BRIGHTNESS_MONITOR);  // Read potentiometer B (for brightness).
  // int my_brightness = map(my_potVal_Brightness, 0, 1023, min_brightness, 255);

  // for (int i = 0; i < 42; i++) {
  //   // ledsInnerLeft[i] = CHSV(my_hue_monitor, 255, my_brightness);  // hue comes from pot A, and brightness value is scaled based on pot B.

  //   ledsInnerLeft[i].setHue( 28);
  // }
  // for (int i = 0; i < NUM_LEDS_MONITOR_LEFT; i++) {
  // // for (int i = 42; i < NUM_LEDS_MONITOR_LEFT; i++) {
  //   // ledsInnerLeft[i] = CHSV(my_hue_monitor, 255, my_brightness);  // hue comes from pot A, and brightness value is scaled based on pot B.

  //   ledsInnerLeft[i].setHue( my_hue_monitor);
  //   // ledsInnerLeft[i].setHue( 224);
  // }




    // int foo_pot_hue = analogRead(DATA_PIN_HUE_MONITOR);

    // int foo_hue_map = map(foo_pot_hue, 0, 1023, 0, 4); 

    // switch (foo_hue_map) {
    //   case 0:
    //     fill_solid(ledsInnerLeft, NUM_LEDS_MONITOR_LEFT, CRGB::Red);
    //     break;
    //   case 1:
    //     fill_solid(ledsInnerLeft, NUM_LEDS_MONITOR_LEFT, CRGB::Blue);
    //     break;
    //   case 2:
    //     fill_solid(ledsInnerLeft, NUM_LEDS_MONITOR_LEFT, CRGB::Green);
    //     break;
    //   case 3:
    //     fill_solid(ledsInnerLeft, NUM_LEDS_MONITOR_LEFT, CRGB::Purple);
    //     break;
    //   case 4:
    //     fill_solid(ledsInnerLeft, NUM_LEDS_MONITOR_LEFT, CRGB::Orange);
    //     break;
    //   default:
    //     fill_solid(ledsInnerLeft, NUM_LEDS_MONITOR_LEFT, CRGB::Brown);
    //     break;
    // }

    FastLED.show();
  }
}

void DOD_PixelStrips::setMonitorHue(){
  for (int i = 0; i < NUM_LEDS_MONITOR_LEFT; i++) {
    // ledsInnerLeft[i] = CHSV(64, 255, 255);  // hue comes from pot A, and brightness value is scaled based on pot B.
    ledsInnerLeft[i] = CHSV(hue_monitor, 255, 255);  // hue comes from pot A, and brightness value is scaled based on pot B.
  }
  // FastLED.setBrightness(128);
  // for (int i = 0; i < NUM_LEDS_MONITOR_RIGHT; i++) {
  //   ledsRightMonitor[i] = CHSV(hue_monitor, 255, 255);  // hue comes from pot A, and brightness value is scaled based on pot B.
  // }
}

void DOD_PixelStrips::checkKnobs(){
  potVal_Hue = analogRead(DATA_PIN_HUE_MONITOR);  // Read potentiometer A (for hue).
  //potValA = map(potValA, 1023, 0, 0, 1023);  // Reverse reading if potentiometer is wired backwards. 
  hue_monitor = map(potVal_Hue, 0, 1023, 0, 255);  // map(value, fromLow, fromHigh, toLow, toHigh)

  potVal_Brightness = analogRead(DATA_PIN_BRIGHTNESS_MONITOR);  // Read potentiometer B (for brightness).
  brightness = map(potVal_Brightness, 0, 1023, min_brightness, 255);
  // brightness = map(potVal_Brightness, 0, 1023, min_brightness, MASTER_BRIGHTNESS);
      // Map value between min_brightness and MASTER brightness values.
      // Note: We are limiting the lowest possible brightness value to the
      // min_brightness value assigned up top.
  // FastLED.setBrightness(brightness);  // Set master brightness based on potentiometer position.

  // DOD_PixelStrips::setMonitorHue();
    for (int i = 0; i < NUM_LEDS_MONITOR_LEFT; i++) {
    // ledsInnerLeft[i] = CHSV(64, 255, 255);  // hue comes from pot A, and brightness value is scaled based on pot B.
    ledsInnerLeft[i] = CHSV(hue_monitor, 255, brightness);  // hue comes from pot A, and brightness value is scaled based on pot B.
    // ledsInnerLeft[i] = CHSV(hue_monitor, 255, 255);  // hue comes from pot A, and brightness value is scaled based on pot B.
  }


  // Serial.print("  pot A: "); Serial.print(potVal_Hue); Serial.print("    hue_monitor: "); Serial.print(hue_monitor);
  //Serial.print("    pot B: "); Serial.print(potValB); Serial.print("    brightness: "); Serial.println(brightness);
}

void DOD_PixelStrips::pacifica_loop() {
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));

  // Clear out the LED array to a dim background blue-green
  fill_solid( ledsLeftDoor, NUM_LEDS_DOOR_LEFT, CRGB( 2, 6, 10));
  fill_solid( ledsRightDoor, NUM_LEDS_DOOR_RIGHT, CRGB( 2, 6, 10));

  // fill_solid( ledsInnerLeft, NUM_LEDS_DOOR_LEFT, CRGB( 2, 6, 10));
  // fill_solid( ledsInnerRight, NUM_LEDS_DOOR_RIGHT, CRGB( 2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

void DOD_PixelStrips::pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff) {
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < NUM_LEDS_DOOR_LEFT; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    ledsLeftDoor[i] += c;
  }
  for( uint16_t i = 0; i < NUM_LEDS_DOOR_RIGHT; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    ledsRightDoor[i] += c;
  }
  // for( uint16_t i = 0; i < NUM_LEDS_INNER_LEFT; i++) {
  //   waveangle += 250;
  //   uint16_t s16 = sin16( waveangle ) + 32768;
  //   uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
  //   ci += cs;
  //   uint16_t sindex16 = sin16( ci) + 32768;
  //   uint8_t sindex8 = scale16( sindex16, 240);
  //   CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
  //   ledsInnerLeft[i] += c;
  // }
  // for( uint16_t i = 0; i < NUM_LEDS_INNER_RIGHT; i++) {
  //   waveangle += 250;
  //   uint16_t s16 = sin16( waveangle ) + 32768;
  //   uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
  //   ci += cs;
  //   uint16_t sindex16 = sin16( ci) + 32768;
  //   uint8_t sindex8 = scale16( sindex16, 240);
  //   CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
  //   ledsInnerRight[i] += c;
  // }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void DOD_PixelStrips::pacifica_add_whitecaps() {
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = 0; i < NUM_LEDS_DOOR_LEFT; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = ledsLeftDoor[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      ledsLeftDoor[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
  for( uint16_t i = 0; i < NUM_LEDS_DOOR_RIGHT; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = ledsRightDoor[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      ledsRightDoor[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
  // for( uint16_t i = 0; i < NUM_LEDS_INNER_LEFT; i++) {
  //   uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
  //   wave += 7;
  //   uint8_t l = ledsInnerLeft[i].getAverageLight();
  //   if( l > threshold) {
  //     uint8_t overage = l - threshold;
  //     uint8_t overage2 = qadd8( overage, overage);
  //     ledsInnerLeft[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
  //   }
  // }
  // for( uint16_t i = 0; i < NUM_LEDS_INNER_RIGHT; i++) {
  //   uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
  //   wave += 7;
  //   uint8_t l = ledsInnerRight[i].getAverageLight();
  //   if( l > threshold) {
  //     uint8_t overage = l - threshold;
  //     uint8_t overage2 = qadd8( overage, overage);
  //     ledsInnerRight[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
  //   }
  // }
}

// Deepen the blues and greens
void DOD_PixelStrips::pacifica_deepen_colors() {
  for( uint16_t i = 0; i < NUM_LEDS_DOOR_LEFT; i++) {
    ledsLeftDoor[i].blue = scale8( ledsLeftDoor[i].blue,  145); 
    ledsLeftDoor[i].green= scale8( ledsLeftDoor[i].green, 200); 
    ledsLeftDoor[i] |= CRGB( 2, 5, 7);
  }
  for( uint16_t i = 0; i < NUM_LEDS_DOOR_RIGHT; i++) {
    ledsRightDoor[i].blue = scale8( ledsRightDoor[i].blue,  145); 
    ledsRightDoor[i].green= scale8( ledsRightDoor[i].green, 200); 
    ledsRightDoor[i] |= CRGB( 2, 5, 7);
  }
  // for( uint16_t i = 0; i < NUM_LEDS_INNER_LEFT; i++) {
  //   ledsInnerLeft[i].blue = scale8( ledsInnerLeft[i].blue,  145); 
  //   ledsInnerLeft[i].green= scale8( ledsInnerLeft[i].green, 200); 
  //   ledsInnerLeft[i] |= CRGB( 2, 5, 7);
  // }
  // for( uint16_t i = 0; i < NUM_LEDS_INNER_RIGHT; i++) {
  //   ledsInnerRight[i].blue = scale8( ledsInnerRight[i].blue,  145); 
  //   ledsInnerRight[i].green= scale8( ledsInnerRight[i].green, 200); 
  //   ledsInnerRight[i] |= CRGB( 2, 5, 7);
  // }
}