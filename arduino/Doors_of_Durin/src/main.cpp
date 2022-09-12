#include <Arduino.h>

#include "pins.h"
#include <DOD_Motor.h>
#include <DOD_Sound.h>
#include <DOD_PixelStrips.h>

#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>


volatile int stopButtonState = 0;           // current state of the stop button (on interrupt)
uint8_t initiateActionButtonState = 0;      // current state of the action button
uint8_t lastInitiateActionButtonState = 0;

int motorState = 0;

const bool MOTORS_ENABLED = true;
const bool SOUND_ENABLED = false;
const bool LEDS_ENABLED = false;

const bool DEBUG = false;
const int debounceTime = 200;

elapsedMillis printTime;
elapsedMillis lastInitiateActionTime;

DOD_PixelStrips *strips = new DOD_PixelStrips();

DOD_Sound *sound = new DOD_Sound(DEBUG);

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
  if (MOTORS_ENABLED) {
    // motor1.stopEverything("Motor 1");
    motor2.stopEverything("Motor 2");
    stepper2->release();
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
        // motor1.initiateAction(initiateActionButtonState);
        motor2.initiateAction(initiateActionButtonState);
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
  // if (DEBUG) {
    Serial.begin(9600);
    // // Serial.begin(115200);
    Serial.println("Start");
    Serial.println();
  // }

  // Setup Emergency STOP button  
  pinMode(DOD_PIN_stopButton, INPUT);
  attachInterrupt(digitalPinToInterrupt(DOD_PIN_stopButton), checkStopButton, RISING);
  
  Serial.print("DOD_PIN_initiateActionButtonPin ");
  Serial.println(DOD_PIN_initiateActionButtonPin);
  pinMode(DOD_PIN_initiateActionButtonPin, INPUT);

  if (SOUND_ENABLED) {
    sound->setup();
  }

  if (LEDS_ENABLED) {
    strips->setupStrips();
  }

  if (MOTORS_ENABLED) {
    AFMS.begin();
    // // setup motors
    // motor1.setupMotor();
    motor2.setupMotor();
  }
}

void loop() {

  checkInitiateActionButton();

  if (MOTORS_ENABLED) {
    // motor1.run();
    motor2.run();

    motorState = motor2.getState();
  }
  
  if (SOUND_ENABLED) {
    sound->run(motorState);
  }

  if (LEDS_ENABLED) {
    strips->loop();
  }

  if (printTime >= 1000) {
    printTime = 0;
    if (MOTORS_ENABLED) {
    //   // motor1.report("Motor 1");
      motor2.report("Motor 2");
    }
  } 
}