#include <Arduino.h>

#include <DOD_Motor.h>
#include <DOD_Sound.h>

#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>


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

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(
  SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);


const uint8_t stopButtonPin = 2; // Button to perform stop everything interrupt
volatile int stopButtonState = 0;     // current state of the stop button
const uint8_t initiateActionButtonPin = 11; 
uint8_t initiateActionButtonState = 0;          // current state of the action button
uint8_t lastInitiateActionButtonState = 0;

const bool DEBUG = false;
const int debounceTime = 200;
const uint8_t motor2Pin_directionToggle = 5;
// const uint8_t motor2Pin_directionToggle = 4;   // now CARDCS
const uint8_t motor2Pin_runningLED = 7;
const uint8_t motor2Pin_closeLimitSwitch = 8;
const uint8_t motor2Pin_moveButton = 9;
const long motor2_targetOpenPosition = 500;  // eventually 7000

// const uint8_t motor1Pin_directionToggle = 4;
// const uint8_t motor1Pin_runningLED = 7;
// const uint8_t motor1Pin_closeOpenButton = 10;
// const uint8_t motor1Pin_closeLimitSwitch = 8;
// const uint8_t motor1Pin_moveButton = 9;

elapsedMillis printTime;
elapsedMillis lastInitiateActionTime;


// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Adafruit_StepperMotor *stepper1 = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *stepper2 = AFMS.getStepper(200, 2);

// void forwardstep1() {
//   stepper1->onestep(FORWARD, DOUBLE);
// }

// void backwardstep1() {
//   stepper1->onestep(BACKWARD, DOUBLE);
// }

void forwardstep2() {
  stepper2->onestep(FORWARD, DOUBLE);
}

void backwardstep2() {
  stepper2->onestep(BACKWARD, DOUBLE);
}


// AccelStepper DOD_Astepper1(forwardstep1, backwardstep1);
AccelStepper DOD_Astepper2(forwardstep2, backwardstep2);

// startingDirection should be opposite for each motor;
// DOD_Motor motor1 = DOD_Motor(
//   DEBUG, 
//   DOD_Astepper1, 
//   motor1Pin_closeOpenButton, 
//   motor1Pin_closeLimitSwitch, 
//   motor1Pin_moveButton, 
//   motor1Pin_directionToggle, 
//   motor1Pin_runningLED,
//   1
// );
DOD_Motor motor2 = DOD_Motor(
  DEBUG, 
  DOD_Astepper2, 
  // motor2Pin_closeOpenButton, 
  motor2Pin_closeLimitSwitch, 
  motor2Pin_moveButton, 
  motor2Pin_directionToggle, 
  motor2Pin_runningLED,
  -1,
  motor2_targetOpenPosition
);

// Interrupt when the STOP button is pressed
// Set state on all motors to STOP
void stopEverything() {
  Serial.println("STOP!");
  // motor1.stopEverything("Motor 1");
  motor2.stopEverything("Motor 2");
  stepper2->release();
}

void checkStopButton() {
  stopButtonState = digitalRead(stopButtonPin);
  if (stopButtonState == HIGH) {
    stopEverything();
  }
}

// When events happen, here is where we communicate to the
// motors, leds, servos, sound, etc.
// called from the main loop()
void checkInitiateActionButton() {
    if ( lastInitiateActionTime >= debounceTime) {
    initiateActionButtonState = digitalRead(initiateActionButtonPin);
    if (initiateActionButtonState != lastInitiateActionButtonState) {
      // motor1.initiateAction(initiateActionButtonState);
      motor2.initiateAction(initiateActionButtonState);
      lastInitiateActionButtonState = initiateActionButtonState;
      lastInitiateActionTime = 0;
    }
  }
}

/// File listing helper
void printDirectory(File dir, int numTabs) {
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

void soundSetup() {
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found !!"));

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
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
}

/**
 * Sound run
 * 
 */
void soundRun() {
  if (! musicPlayer.startPlayingFile("/track001.mp3")) {
    Serial.println("Could not open file track001.mp3");
    while (1);
  }
  Serial.println(F("Started playing !!"));

  while (musicPlayer.playingMusic) {
    // file is now playing in the 'background' so now's a good time
    // to do something else like handling LEDs or buttons :)
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Done playing music");
}

/**
 * Main Setup
 * 
 */

void setup() {
  // if (DEBUG) {
    Serial.begin(9600);
    // // Serial.begin(115200);
    Serial.println("Start");
    Serial.println();
  // }

  // Setup Emergency STOP button  
  pinMode(stopButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(stopButtonPin), checkStopButton, RISING);
  
  pinMode(initiateActionButtonPin, INPUT);

  AFMS.begin();

  soundSetup();



  // // setup motors
  // motor1.setupMotor();
  motor2.setupMotor();
}

void loop() {
  soundRun();

  checkInitiateActionButton();

  // motor1.run();
  motor2.run();

  if (printTime >= 1000) {
    printTime = 0;
  //   // motor1.report("Motor 1");
    motor2.report("Motor 2");
  } 
}