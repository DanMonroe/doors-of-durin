#include "DOD_Sound.h"
#include "motorStates.h"
#include "pins.h"

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// #define BREAKOUT_RESET  9      // VS1053 reset pin (output)
// #define BREAKOUT_CS     10     // VS1053 chip select pin (output)
// #define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// // These are the pins used for the music maker shield
// #define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
// #define SHIELD_CS     7      // VS1053 chip select pin (output)
// #define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// // These are common pins between breakout and shield
// #define CARDCS 4     // Card chip select pin
// // DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
// #define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(
  SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

DOD_Sound::DOD_Sound() {
}

void DOD_Sound::setup() {
  Serial.println("Sound Setup in class");

  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present !!"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);

  /***** Two interrupt options! *******/ 
  // This option uses timer0, this means timer1 & t2 are not required
  // (so you can use 'em for Servos, etc) BUT millis() can lose time
  // since we're hitchhiking on top of the millis() tracker
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  // *** This method is preferred
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) {
    Serial.println(F("DREQ pin is not an interrupt pin"));
  }
}

void DOD_Sound::run(int motorState) {

  if (lastMotorState != motorState) {
    Serial.println("Motor state changed... check sound to play");

    switch (motorState) {
      case MOTOR_STATE_OPENING:
        if (musicPlayer.playingMusic) {
            Serial.println("Stop playing before starting to play");
            musicPlayer.stopPlaying();
            delay(10);
            // musicPlayer.softReset();
          }
          if (! musicPlayer.startPlayingFile("/track001.mp3")) {
            Serial.println("Could not open file ");
          }

        Serial.println(F("Started playing door opening sound"));
        break;
      default:
          if (musicPlayer.playingMusic) {
            Serial.println("Stop playing");
            musicPlayer.stopPlaying();
            delay(10);
            // musicPlayer.softReset();
          }

        break;
    }
  // while (musicPlayer.playingMusic) {
  //   // file is now playing in the 'background' so now's a good time
  //   // to do something else like handling LEDs or buttons :)
  //   Serial.print(".");
  //   delay(1000);
  // }
  // Serial.println("Done playing music");

    // Keep track of last motor state so we can detect changes
    lastMotorState = motorState;
  }

}

void DOD_Sound::playSound(const char * soundName) {
  stopIfPlaying();

  if (! musicPlayer.startPlayingFile(soundName)) {
    Serial.print("Could not open file ");
    Serial.println(soundName);
  }
}

void DOD_Sound::stopIfPlaying() {
  if (musicPlayer.playingMusic) {
    Serial.println("Stop playing");
    musicPlayer.stopPlaying();
  }
}

/// File listing helper
void DOD_Sound::printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
      //  Don't bother displaying sub directories for now - Dan
      //  Serial.println("/");
      //  printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}


