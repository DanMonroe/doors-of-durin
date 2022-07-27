#ifndef _DOD_pins_h
#define _DOD_pins_h

#include <Arduino.h>
/**
 * Sound definitions
 * 
 */
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

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