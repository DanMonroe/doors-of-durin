#include "DOD_PixelStrips.h"
#include "DOD_Pixel.h"

#include <Arduino.h>

#define FASTLED_INTERNAL        // Suppress build banner

#define NUM_LEDS_DOOR_LEFT 287
#define NUM_LEDS_DOOR_RIGHT 276
#define DATA_PIN_DOOR_LEFT A0
#define DATA_PIN_DOOR_RIGHT A1

#define NUM_LEDS_MONITOR_LEFT 84
#define NUM_LEDS_SYMBOL_LEFT 20
#define NUM_LEDS_INNER_LEFT 104

#define NUM_LEDS_MONITOR_RIGHT 85
#define NUM_LEDS_SYMBOL_RIGHT 20
#define NUM_LEDS_INNER_RIGHT 105

#define DATA_PIN_INNER_LEFT A2
#define DATA_PIN_INNER_RIGHT A3

#define DATA_PIN_HUE_MONITOR A7
#define DATA_PIN_BRIGHTNESS_MONITOR A6

uint8_t MASTER_BRIGHTNESS = 255;
#define SYMBOLS_BRIGHTNESS 100;

// https://www.youtube.com/watch?v=Ukq0tH2Tnkc - palettes
// http://fastled.io/tools/paletteknife/
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/views/totp-c3g.html
// pos, red, green, black
// DEFINE_GRADIENT_PALETTE( firePalette_gp ) {
//   0,    250,  65,   0,      // red/orange
//   64,   230,  250,  0,      // yellow
//   128,  255,  177,  0,      // light orange
//   // 255,  0,  0,   0,      // black
//   255,  250,  65,   1,      // red/orange
// };

// Gradient palette "fire_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/neota/elem/tn/fire.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

// DEFINE_GRADIENT_PALETTE( fire_gp ) {
//     0,   0,  0,  0,
//    76,  32,  5,  0,
//   146, 192, 24,  0,
//   197, 220,105,  5,
//   240, 252,255, 31,
//   250, 252,255,111,
//   255, 255,255,255};

// Gradient palette "bhw2_xc_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw2/tn/bhw2_xc.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.
// fire with some purple
DEFINE_GRADIENT_PALETTE( bhw2_xc_gp ) {
    0,   4,  2,  9,
   58,  16,  0, 47,
  122,  24,  0, 16,
  158, 144,  9,  1,
  183, 179, 45,  1,
  219, 220,114,  2,
  255, 234,237,  1};

CRGBPalette16 rightSymbolPallette = bhw2_xc_gp;

// CRGBPalette16 firePallette = fire_gp;
// CRGBPalette16 firePallette = firePalette_gp;
uint8_t ringSymbolColorIndex[NUM_LEDS_SYMBOL_RIGHT];

  // 0,    39,   87,   40,
DEFINE_GRADIENT_PALETTE( monroe_earth_gp ) {
  0,    3,   3,   3,
  36,   249,  186,  45,
  59,   202,  138,  2,
  112,  41,   213,  3,
  255,  0,    133,  10};

// Gradient palette "girlcat_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/rc/tn/girlcat.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.
// green earthy
// DEFINE_GRADIENT_PALETTE( girlcat_gp ) {
//     0, 173,229, 51,
//    73, 139,199, 45,
//   140,  46,176, 37,
//   204,   7, 88,  5,
//   255,   0, 24,  0};
CRGBPalette16 leftSymbolPallette = monroe_earth_gp;

int potVal_Hue;         // Variable to store potentiometer A value (for hue)
int potVal_Brightness;  // Variable to store potentiometer B value (for brightness)

int last_hue = -1;
int last_brightness = -1;


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

// Outside
CRGB ledsLeftDoor[NUM_LEDS_DOOR_LEFT];
CRGB ledsRightDoor[NUM_LEDS_DOOR_RIGHT];

// Inside
CRGB realLedsInnerLeft[NUM_LEDS_INNER_LEFT];
CRGBSet allledsleft(realLedsInnerLeft, NUM_LEDS_INNER_LEFT);
CRGBSet ledsInnerLeft(allledsleft(0, NUM_LEDS_INNER_LEFT));
// CRGBSet ledsMonitorRight(ledsInnerRight, NUM_LEDS_MONITOR_RIGHT);
CRGBSet ledsSymbolLeft(allledsleft(NUM_LEDS_MONITOR_LEFT, NUM_LEDS_MONITOR_LEFT));
CRGB realLedsInnerRight[NUM_LEDS_INNER_RIGHT];
CRGBSet allledsright(realLedsInnerRight, NUM_LEDS_INNER_RIGHT);
CRGBSet ledsInnerRight(allledsright(0, NUM_LEDS_INNER_RIGHT));
// CRGBSet ledsMonitorRight(ledsInnerRight, NUM_LEDS_MONITOR_RIGHT);
CRGBSet ledsSymbolRight(allledsright(NUM_LEDS_MONITOR_RIGHT, NUM_LEDS_MONITOR_RIGHT));


void DOD_PixelStrips::setupStrips() {
  // Serial.println("setupStrips");

  // limit my draw to 20A at 5v of power draw
  FastLED.setMaxPowerInVoltsAndMilliamps(5,20000); 

  FastLED.setBrightness( MASTER_BRIGHTNESS );

  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_LEFT, GRB>(ledsLeftDoor, NUM_LEDS_DOOR_LEFT).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_RIGHT, GRB>(ledsRightDoor, NUM_LEDS_DOOR_RIGHT).setCorrection( TypicalLEDStrip );

  FastLED.addLeds<WS2812B, DATA_PIN_INNER_LEFT, GRB>(ledsInnerLeft, NUM_LEDS_INNER_LEFT).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812B, DATA_PIN_INNER_RIGHT, GRB>(ledsInnerRight, NUM_LEDS_INNER_RIGHT).setCorrection( TypicalLEDStrip );

  fill_solid(ledsLeftDoor, NUM_LEDS_DOOR_LEFT, CRGB::Black);
  fill_solid(ledsRightDoor, NUM_LEDS_DOOR_RIGHT, CRGB::Black);
  
  fill_solid(ledsInnerLeft, NUM_LEDS_INNER_LEFT, CRGB::Purple);
  fill_solid(ledsInnerRight, NUM_LEDS_INNER_RIGHT, CRGB::Purple);
  // fill_solid(ledsInnerLeft, NUM_LEDS_INNER_LEFT, CRGB::Black);
  // fill_solid(ledsInnerRight, NUM_LEDS_INNER_RIGHT, CRGB::Black);
 
  
  FastLED.show();

  // pick random color from fire pallette to start with
  for (int i = 0; i < NUM_LEDS_SYMBOL_RIGHT; i++) {
    ringSymbolColorIndex[i] = random8();
  }

  // FastLED.clear();
}

void DOD_PixelStrips::loop() {
  // DOD_PixelStrips::matrixDemo();

  DOD_PixelStrips::symbols();
  EVERY_N_MILLISECONDS(20) {
    DOD_PixelStrips::pacifica_loop();
  }
  FastLED.show();
  // EVERY_N_MILLISECONDS(1000) {
    // DOD_PixelStrips::checkMonitorControls();
    // random16_add_entropy( random());  // fire
  // }
}


// Symbols
void DOD_PixelStrips::symbols() {
  for (int i = 0; i < NUM_LEDS_SYMBOL_LEFT; i++) {
    ledsSymbolLeft[i] = ColorFromPalette(leftSymbolPallette, ringSymbolColorIndex[i]);
  }

  for (int i = 0; i < NUM_LEDS_SYMBOL_RIGHT; i++) {
    ledsSymbolRight[i] = ColorFromPalette(rightSymbolPallette, ringSymbolColorIndex[i]);
  }

  EVERY_N_MILLISECONDS(5) {
    for(int i = 0; i < NUM_LEDS_SYMBOL_RIGHT; i++) {
      ringSymbolColorIndex[i]++;
    }
  }  
}
// End Symbols

void DOD_PixelStrips::checkMonitorControls(){
  int my_potVal_hue = analogRead(DATA_PIN_HUE_MONITOR);  // Read potentiometer for hue.
  int my_potVal_brightness = analogRead(DATA_PIN_BRIGHTNESS_MONITOR);  // Read potentiometer for brightness.

  CHSV spectrumcolor;
  spectrumcolor.saturation = 	255;

  if((my_potVal_hue != last_hue) || (my_potVal_brightness != last_brightness)) {
    int this_hue_monitor = map(my_potVal_hue, 0, 1023, 0, 255);  // map(value, fromLow, fromHigh, toLow, toHigh)
    int this_brightness_monitor = map(my_potVal_brightness, 0, 1023, 0, 255);  // map(value, fromLow, fromHigh, toLow, toHigh)
    spectrumcolor.hue = this_hue_monitor;
    spectrumcolor.value = this_brightness_monitor;
    last_hue = my_potVal_hue;
    last_brightness = my_potVal_brightness;
    for (int i = 0; i < NUM_LEDS_MONITOR_LEFT; i++) {
      hsv2rgb_spectrum( spectrumcolor, ledsInnerLeft[i] );
    }
    for (int i = 0; i < NUM_LEDS_MONITOR_RIGHT; i++) {
      hsv2rgb_spectrum( spectrumcolor, ledsInnerRight[i] );
    }
  }
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
  
  // Render each of four layers, with different scales and speeds, that vary over time
  
  // left door front
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301), NUM_LEDS_DOOR_LEFT, ledsLeftDoor, 0);
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401), NUM_LEDS_DOOR_LEFT, ledsLeftDoor, 0);
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503), NUM_LEDS_DOOR_LEFT, ledsLeftDoor, 0);
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601), NUM_LEDS_DOOR_LEFT, ledsLeftDoor, 0);
  // right door front
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301), NUM_LEDS_DOOR_RIGHT, ledsRightDoor, 0);
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401), NUM_LEDS_DOOR_RIGHT, ledsRightDoor, 0);
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503), NUM_LEDS_DOOR_RIGHT, ledsRightDoor, 0);
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601), NUM_LEDS_DOOR_RIGHT, ledsRightDoor, 0);
  
  // Add brighter 'whitecaps' where the waves lines up more
  // pacifica_add_whitecaps(NUM_LEDS_DOOR_LEFT, ledsLeftDoor);
  // pacifica_add_whitecaps(NUM_LEDS_DOOR_RIGHT, ledsRightDoor, 0);

  // Deepen the blues and greens a bit
  pacifica_deepen_colors(NUM_LEDS_DOOR_LEFT, ledsLeftDoor, 0);
  pacifica_deepen_colors(NUM_LEDS_DOOR_RIGHT, ledsRightDoor, 0);
}

void DOD_PixelStrips::pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff, uint16_t numberOfLeds, CRGB *leds, uint16_t startLed ) {
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = startLed; i < (startLed + numberOfLeds); i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void DOD_PixelStrips::pacifica_add_whitecaps(uint16_t numberOfLeds, CRGB *leds, uint16_t startLed) {
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = startLed; i < (startLed + numberOfLeds); i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}
// Deepen the blues and greens
void DOD_PixelStrips::pacifica_deepen_colors(uint16_t numberOfLeds, CRGB *leds, uint16_t startLed) {
  for( uint16_t i = startLed; i < (startLed + numberOfLeds); i++) {
    leds[i].blue = scale8( leds[i].blue,  145); 
    leds[i].green= scale8( leds[i].green, 200); 
    leds[i] |= CRGB( 2, 5, 7);
  }
}
