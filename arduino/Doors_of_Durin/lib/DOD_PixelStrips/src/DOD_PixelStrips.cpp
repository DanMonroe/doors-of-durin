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

// DEFINE_GRADIENT_PALETTE( firePal_gp ) {
//   100,  100,  0,    0,      //dark red
//   128,  255,  0,    0,      //red
//   220,  255,  255,  0,      //yellow
//   255,  255,  255,  255 };  //white
// DEFINE_GRADIENT_PALETTE( firePal_gp ) {
//   0,    0,    0,    0,      //black
//   100,  100,  0,    0,      //dark red
//   128,  255,  0,    0,      //red
//   220,  255,  255,  0,      //yellow
//   255,  255,  255,  255 };  //white











int potVal_Hue;         // Variable to store potentiometer A value (for hue)
int potVal_Brightness;  // Variable to store potentiometer B value (for brightness)

int last_hue = -1;
int last_brightness = -1;

// bool gReverseDirection = false;

extern const TProgmemRGBPalette16 LavaColors_p FL_PROGMEM;
// extern const TProgmemRGBPalette16 HeatColors_p FL_PROGMEM;
// extern const TProgmemRGBPalette16 CloudColors_p FL_PROGMEM;
extern const TProgmemRGBPalette16 ForestColors_p FL_PROGMEM;
CRGBPalette16 lava_colors_1 = CRGBPalette16(LavaColors_p);
CRGBPalette16 lava_colors_2 = CRGBPalette16(HeatColors_p);
CRGBPalette16 lava_colors_3 = CRGBPalette16(CloudColors_p);
CRGBPalette16 forest_colors_1 = CRGBPalette16(ForestColors_p);

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

// CRGBPalette16 fire_palette = CRGBPalette16(firePal_gp);

CRGB ledsLeftDoor[NUM_LEDS_DOOR_LEFT];
CRGB ledsRightDoor[NUM_LEDS_DOOR_RIGHT];

CRGB ledsInnerLeft[NUM_LEDS_INNER_LEFT];
CRGB realLedsInnerRight[NUM_LEDS_INNER_RIGHT];
CRGBSet leds(realLedsInnerRight, NUM_LEDS_INNER_RIGHT);
CRGBSet ledsInnerRight(leds(0, NUM_LEDS_INNER_RIGHT));
// CRGBSet ledsMonitorRight(ledsInnerRight, NUM_LEDS_MONITOR_RIGHT);
CRGBSet ledsSymbolRight(leds(NUM_LEDS_MONITOR_RIGHT, NUM_LEDS_MONITOR_RIGHT));


// Params for width and height
const uint8_t kMatrixWidth = 5;
const uint8_t kMatrixHeight = 4;

// First LED is lower left
  const uint8_t RightXYTable[] = {
    19,  18,  17,  16,  15,
    10,  11,  12,  13,  14,
     9,   8,   7,   6,   5,
     0,   1,   2,   3,   4
  };

// #define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// CRGB leds[ NUM_LEDS ];
#define LAST_VISIBLE_SYMBOLS_LED_BASE 19
uint8_t XY (uint8_t x, uint8_t y, bool isLeftSide) {

  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (isLeftSide == true) ? (LAST_VISIBLE_SYMBOLS_LED_BASE + 1) : (LAST_VISIBLE_SYMBOLS_LED_BASE + 1);
  }

  // First LED is lower right
  const uint8_t LeftXYTable[] = {
    15,  16,  17,  18,  19,
    14,  13,  12,  11,  10,
     5,   6,   7,   8,   9,
     4,   3,   2,   1,   0
  };

  // // First LED is lower left
  // const uint8_t RightXYTable[] = {
  //   19,  18,  17,  16,  15,
  //   10,  11,  12,  13,  14,
  //    9,   8,   7,   6,   5,
  //    0,   1,   2,   3,   4
  // };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = (isLeftSide == true) ? LeftXYTable[i] + NUM_LEDS_MONITOR_LEFT : RightXYTable[i] + (NUM_LEDS_MONITOR_RIGHT);
  return j;
}





void DOD_PixelStrips::setupStrips() {
  // Serial.println("setupStrips");

  // limit my draw to 20A at 5v of power draw
  FastLED.setMaxPowerInVoltsAndMilliamps(5,20000); 

  FastLED.setBrightness( MASTER_BRIGHTNESS );

  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_LEFT, GRB>(ledsLeftDoor, NUM_LEDS_DOOR_LEFT).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812B, DATA_PIN_DOOR_RIGHT, GRB>(ledsRightDoor, NUM_LEDS_DOOR_RIGHT).setCorrection( TypicalLEDStrip );

  FastLED.addLeds<WS2812B, DATA_PIN_INNER_LEFT, GRB>(ledsInnerLeft, NUM_LEDS_INNER_LEFT).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812B, DATA_PIN_INNER_RIGHT, GRB>(ledsInnerRight, NUM_LEDS_INNER_RIGHT).setCorrection( TypicalLEDStrip );

  // FastLED.addLeds<WS2812B, DATA_PIN_INNER_RIGHT, GRB>(ledsMonitorRight, 0, NUM_LEDS_MONITOR_RIGHT).setCorrection( TypicalLEDStrip );
  // FastLED.addLeds<WS2812B, DATA_PIN_INNER_RIGHT, GRB>(ledsSymbolRight, NUM_LEDS_MONITOR_RIGHT, NUM_LEDS_SYMBOL_RIGHT).setCorrection( TypicalLEDStrip );

  fill_solid(ledsLeftDoor, NUM_LEDS_DOOR_LEFT, CRGB::Black);
  fill_solid(ledsRightDoor, NUM_LEDS_DOOR_RIGHT, CRGB::Black);
  
  fill_solid(ledsInnerLeft, NUM_LEDS_INNER_LEFT, CRGB::Purple);
  fill_solid(ledsInnerRight, NUM_LEDS_INNER_RIGHT, CRGB::Purple);
  // fill_solid(ledsMonitorRight, NUM_LEDS_MONITOR_RIGHT, CRGB::Purple);
  // fill_solid(ledsSymbolRight, NUM_LEDS_SYMBOL_RIGHT, CRGB::Red);

  // fill_solid(ledsMonitorRight, NUM_LEDS_MONITOR_RIGHT, CRGB::Pink);
  // fill_solid(ledsInnerLeft, NUM_LEDS_INNER_LEFT, CRGB::Black);
  // fill_solid(ledsInnerRight, NUM_LEDS_INNER_RIGHT, CRGB::Black);
 
  // for( uint8_t i = NUM_LEDS_SYMBOL_LEFT; i < NUM_LEDS_INNER_LEFT; i++) {
  //   ledsInnerLeft[i]  = CRGB::Black;
  // }
  // for( uint8_t i = NUM_LEDS_SYMBOL_RIGHT; i < NUM_LEDS_INNER_RIGHT; i++) {
  //   ledsInnerRight[i]  = CRGB::Black;
  // }

  // FastLED.clear();
  FastLED.show();
}

void DOD_PixelStrips::loop() {
  // DOD_PixelStrips::matrixDemo();

  DOD_PixelStrips::symbols();
  EVERY_N_MILLISECONDS(20) {
    DOD_PixelStrips::pacifica_loop();

  }
  FastLED.show();
  // EVERY_N_MILLISECONDS(1000) {
    // DOD_PixelStrips::symbols();
    // DOD_PixelStrips::checkMonitorControls();
    // random16_add_entropy( random());  // fire
    // DOD_PixelStrips::pacifica_loop();

  //   // Fourth, the most sophisticated: this one sets up a new palette every
  // // time through the loop, based on a hue that changes every time.
  // // The palette is a gradient from black, to a dark color based on the hue,
  // // to a light color based on the hue, to white.
  // //
  //   static uint8_t hue = 0;
  //   hue++;
  //   CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
  //   CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
  //   fire_palette = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);

  //   fire2012WithPalette(); // run simulation frame, using palette colors

    // FastLED.show();
  // }

}


// Symbols
void DOD_PixelStrips::symbols() {

  
  uint8_t spiralIds[] = {
    XY(1, 0, false),
    XY(2, 0, false),
    XY(3, 0, false),
    XY(4, 1, false),
    XY(4, 2, false),
    XY(3, 3, false),
    XY(2, 3, false),
    XY(1, 3, false),
    XY(0, 2, false),
    XY(0, 1, false)
  };

  uint8_t sinBeat = beatsin8(30, 0, 9, 0, 0);
  uint8_t sinBeat2 = beatsin8(60, 0, 9, 0, 0);

  uint8_t sinBeat3 = beatsin8(30, 0, 9, 0, 127);
  uint8_t sinBeat4 = beatsin8(60, 0, 9, 0, 127);

  // Color
  uint8_t colBeat = beatsin8(45, 0, 255, 0, 0);

  ledsInnerRight[spiralIds[(sinBeat + sinBeat2) / 2]] = CHSV(colBeat, 255, 255);
  ledsInnerRight[spiralIds[(sinBeat3 + sinBeat4) / 2]] = CHSV(colBeat, 255, 255);
  
  // EVERY_N_MILLISECONDS(1) {
  //   for(int i = 0; i < 4; i++) {
  //     blur1d(ledsSymbolRight, NUM_LEDS_SYMBOL_RIGHT, 50);
  //   }
  // }
  
  
  fadeToBlackBy(ledsSymbolRight, NUM_LEDS_SYMBOL_RIGHT, 10);

  
}

// Done Symbols


// void DOD_PixelStrips::matrixDemo() {
//   uint32_t ms = millis();
//     int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
//     int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));
//     DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
//     if( ms < 5000 ) {
//       FastLED.setBrightness( scale8( 100, (ms * 256) / 5000)); // 100 brightness
//     } else {
//       FastLED.setBrightness(100);
//     }
//     FastLED.show();
// }
// void DOD_PixelStrips::DrawOneFrame( uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
//   uint8_t lineStartHue = startHue8;
//   for( uint8_t y = 0; y < kMatrixHeight; y++) {
//     lineStartHue += yHueDelta8;
//     uint8_t pixelHue = lineStartHue;      
//     for( uint8_t x = 0; x < kMatrixWidth; x++) {
//       pixelHue += xHueDelta8;
//       // ledsInnerLeft[ XY(x, y, true)]  = CHSV( pixelHue, 255, 255);
//       ledsInnerRight[ XY(x, y, false)]  = CHSV( pixelHue, 255, 255);
//     }
//   }
// }


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

  // for (uint16_t i = 0; i < NUM_LEDS_SYMBOL_LEFT; i++) {
  //   ledsInnerLeft[NUM_LEDS_MONITOR_LEFT + i] = CRGB( 0, 255, 0);
  //   ledsInnerRight[NUM_LEDS_MONITOR_RIGHT + i] = CRGB( 255, 0, 0);
  // }

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
  // left symbol
  // // pacifica_one_layer( forest_colors_1, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401), NUM_LEDS_SYMBOL_LEFT, ledsInnerLeft, NUM_LEDS_MONITOR_LEFT);
  // pacifica_one_layer( fire_palette, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401), NUM_LEDS_SYMBOL_LEFT, ledsInnerLeft, NUM_LEDS_MONITOR_LEFT);
  // pacifica_one_layer( lava_colors_1, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401), NUM_LEDS_SYMBOL_LEFT, ledsInnerLeft, NUM_LEDS_MONITOR_LEFT);
  // pacifica_one_layer( lava_colors_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401), NUM_LEDS_SYMBOL_LEFT, ledsInnerLeft, NUM_LEDS_MONITOR_LEFT);
  // pacifica_one_layer( lava_colors_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503), NUM_LEDS_SYMBOL_LEFT, ledsInnerLeft, NUM_LEDS_MONITOR_LEFT);
  // pacifica_one_layer( lava_colors_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601), NUM_LEDS_SYMBOL_LEFT, ledsInnerLeft, NUM_LEDS_MONITOR_LEFT);

  // pacifica_one_layer( fire_palette, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401), NUM_LEDS_SYMBOL_RIGHT, ledsInnerRight, NUM_LEDS_MONITOR_RIGHT);

  // Add brighter 'whitecaps' where the waves lines up more
  // pacifica_add_whitecaps(NUM_LEDS_DOOR_LEFT, ledsLeftDoor);
  // pacifica_add_whitecaps(NUM_LEDS_DOOR_RIGHT, ledsRightDoor, 0);

  // Deepen the blues and greens a bit
  pacifica_deepen_colors(NUM_LEDS_DOOR_LEFT, ledsLeftDoor, 0);
  pacifica_deepen_colors(NUM_LEDS_DOOR_RIGHT, ledsRightDoor, 0);
  // pacifica_deepen_colors(NUM_LEDS_SYMBOL_LEFT, ledsInnerLeft, NUM_LEDS_SYMBOL_LEFT);
  // pacifica_deepen_colors(NUM_LEDS_SYMBOL_RIGHT, ledsInnerRight, NUM_LEDS_SYMBOL_RIGHT);
  // pacifica_deepen_colors();
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
