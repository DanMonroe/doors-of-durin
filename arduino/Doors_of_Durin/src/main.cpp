#include <Arduino.h>

#include "pins.h"
// #include <DOD_Motor.h>
#include <DOD_Sound.h>
#include <DOD_PixelStrips.h>

// #include <AccelStepper.h>
#include <Wire.h>
// #include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>


volatile int stopButtonState = 0;           // current state of the stop button (on interrupt)
uint8_t initiateActionButtonState = 0;      // current state of the action button
uint8_t lastInitiateActionButtonState = 0;

// int motorState = 0;

const bool MOTORS_ENABLED = true;
const bool SOUND_ENABLED = false;
const bool LEDS_ENABLED = true;

const bool DEBUG = false;
const int debounceTime = 200;

// elapsedMillis printTime;
elapsedMillis lastInitiateActionTime;

DOD_PixelStrips *strips = new DOD_PixelStrips();

DOD_Sound *sound = new DOD_Sound(DEBUG);

// Interrupt when the STOP button is pressed
// Set state on all motors to STOP
void stopEverything() {
  if (DEBUG) {
    Serial.println("STOP!");
  }
  if (MOTORS_ENABLED) {
    // leftMotor.stopEverything("Motor 1");
    // rightMotor.stopEverything("Motor 2");
    // rightStepper->release();

    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(SIGNAL_STOP_EVERYTHING);
    Wire.endTransmission();

  }
}

void checkStopButton() {
  stopButtonState = digitalRead(DOD_PIN_stopButton);
  if (stopButtonState == HIGH) {
    stopEverything();
  }
}

// When events happen, here is where we communicate to the
// motors, leds, servos, sound, etc.
// called from the main loop()
void checkInitiateActionButton() {
    if ( lastInitiateActionTime >= debounceTime) {
    initiateActionButtonState = digitalRead(DOD_PIN_initiateActionButtonPin);
    if (initiateActionButtonState != lastInitiateActionButtonState) {
      if (MOTORS_ENABLED) {
        Wire.beginTransmission(SLAVE_ADDR);
        if (initiateActionButtonState == HIGH) {
          if (DEBUG) {
            Serial.println("checkInitiateActionButton writing 200");
          }
          Wire.write(SIGNAL_INITIATE_BUTTON_HIGH);
        } else {
          if (DEBUG) {
            Serial.println("checkInitiateActionButton writing 201");
          }
          Wire.write(SIGNAL_INITIATE_BUTTON_LOW);
        }
        Wire.endTransmission();

        // leftMotor.initiateAction(initiateActionButtonState);
        // rightMotor.initiateAction(initiateActionButtonState);
      } else {
        if (DEBUG) {
          Serial.println("motors disabled");
        }
      }
      lastInitiateActionButtonState = initiateActionButtonState;
      lastInitiateActionTime = 0;
    }
  }
}


/**
 * Main Setup
 * 
 */

void setup() {


  if (MOTORS_ENABLED) {
    // Initialize I2C communications as Master
    Wire.begin();
  }

  if (DEBUG) {
    Serial.begin(9600);
    // // Serial.begin(115200);
    Serial.println("Start");
    Serial.println();
  }

  // Setup Emergency STOP button  
  pinMode(DOD_PIN_stopButton, INPUT);
  attachInterrupt(digitalPinToInterrupt(DOD_PIN_stopButton), checkStopButton, RISING);
  
  pinMode(DOD_PIN_initiateActionButtonPin, INPUT_PULLUP);

  if (SOUND_ENABLED) {
    sound->setup();
  }

  if (LEDS_ENABLED) {
    strips->setupStrips();
  }

  // if (MOTORS_ENABLED) {
  //   AFMS.begin();
  //   // // setup motors
  //   // leftMotor.setupMotor();
  //   rightMotor.setupMotor();
  // }
}

void loop() {

  checkInitiateActionButton();

  if (LEDS_ENABLED) {
    strips->loop();
  }

  // if (MOTORS_ENABLED) {
  //   Wire.beginTransmission(SLAVE_ADDR);
  //   Wire.write(1);
  //   Wire.endTransmission();

  // //   // leftMotor.run();
  // //   rightMotor.run();

  // //   motorState = rightMotor.getState();
  // }
  
  // if (SOUND_ENABLED) {
  //   sound->run(motorState);
  // }

  // if (LEDS_ENABLED) {
  //   strips->loop();
  // }

  // if (printTime >= 1000) {
  //   printTime = 0;
  //   if (MOTORS_ENABLED) {
  //   //   // leftMotor.report("Motor 1");
  //     rightMotor.report("Motor 2");
  //   }
  // } 
}