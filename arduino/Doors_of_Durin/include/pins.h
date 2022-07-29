#ifndef _DOD_pins_h
#define _DOD_pins_h

#include <Arduino.h>
/**
 * Sound definitions
 * 
 * https://learn.adafruit.com/adafruit-music-maker-shield-vs1053-mp3-wav-wave-ogg-vorbis-player/pinouts
 * 
 * There are three 'totally fixed' pins, the hardware SPI pins:
 * SPI SCK - connected to Digital #13 (but can be connected to the ISP header with a jumper) - used by both the SD card and VS1053
 * SPI MISO - connected to Digital #12 (but can be connected to the ISP header with a jumper) - used by both the SD card and VS1053
 * SPI MOSI - connected to Digital #11 (but can be connected to the ISP header with a jumper) - used by both the SD card and VS1053
 * There are a couple other pins that are required for talking to the VS1053 to play MP3s and such
 * MCS - this is the VS1053 chip select pin, connected to Digital #7
 * DCS - this is the VS1053 data select pin, connected to Digital #6
 * CCS - this is the SD Card chip select pin, connected to Digital #4
 * DREQ - this is the VS1053 data request interrupt pin - connected to digital #3
 * 
 * Note pins 11, 12, 13 taken above
 */
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET    -1      // VS1053 reset pin (unused!)
#define SHIELD_CS       7      // VS1053 chip select pin (output)
#define SHIELD_DCS      6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS          4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ            3       // VS1053 Data request, ideally an Interrupt pin

/**
 * Main
 * 
 */
const int DOD_PIN_stopButton = 2;               // Button to perform stop everything interrupt
const int DOD_PIN_initiateActionButtonPin = 14; 

/**
 * Motors
 * 
 */
const uint8_t motor2Pin_directionToggle = 5;
const uint8_t motor2Pin_runningLED = 7;
const uint8_t motor2Pin_closeLimitSwitch = 8;
const uint8_t motor2Pin_moveButton = 9;
const long motor2_targetOpenPosition = 500;  // eventually 7000


#endif