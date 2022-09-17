#include <Arduino.h>

#include "pins.h"
// #include <DOD_PixelStrips.h>
#include <Wire.h>
#include <DOD_Motor.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

const bool MOTORS_ENABLED = true;
const bool DEBUG = false;

int motorState1 = 0;
int motorState2 = 0;
int wireState = 0;
int lastWireState = 0;

elapsedMillis printTime;

void receiveEvent(int signalCode) {
  // Read while data received
  while (0 < Wire.available()) {
    wireState = Wire.read();
    if (DEBUG) {
      Serial.print("receiveEvent wireState ");
      Serial.println(wireState);
    }
  }
}
 
// void requestEvent() {
//   // Setup byte variable in the correct size
//   byte response[ANSWERSIZE];
//   // Format answer as array
//   for (byte i=0;i<ANSWERSIZE;i++) {
//     response[i] = (byte)answer.charAt(i);
//   }
//   // Send response back to Master
//   Wire.write(response,sizeof(response));
//   // Print to Serial Monitor
//   Serial.println("Request event");
// }

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Adafruit_StepperMotor *leftStepper = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *rightStepper = AFMS.getStepper(200, 1);

// void leftForwardStep() {
//   leftStepper->onestep(FORWARD, DOUBLE);
// }
// void leftBackwardStep() {
//   leftStepper->onestep(BACKWARD, DOUBLE);
// }

void rightForwardStep() {
  rightStepper->onestep(FORWARD, DOUBLE);
}

void rightBackwardstep() {
  rightStepper->onestep(BACKWARD, DOUBLE);
}


// AccelStepper DOD_leftStepper(leftForwardStep, leftBackwardStep);
AccelStepper DOD_rightStepper(rightForwardStep, rightBackwardstep);

// startingDirection should be opposite for each motor;
// DOD_Motor leftMotor = DOD_Motor(
//   "Motor 1",
//   DEBUG, 
//   DOD_leftStepper, 
//   // leftMotorPin_closeOpenButton, 
//   leftMotorPin_closeLimitSwitch, 
//   // leftMotorPin_moveButton, 
//   // leftMotorPin_directionToggle, 
//   // leftMotorPin_runningLED,
//   1,
//   leftMotor_targetOpenPosition
// );
DOD_Motor rightMotor = DOD_Motor(
  "Motor 2",
  DEBUG, 
  DOD_rightStepper, 
  // rightMotorPin_closeOpenButton, 
  rightMotorPin_closeLimitSwitch, 
  // rightMotorPin_moveButton, 
  // rightMotorPin_directionToggle, 
  // rightMotorPin_runningLED,
  -1,
  rightMotor_targetOpenPosition
);



void setup() {

  // Initialize I2C communications as Slave
  Wire.begin(SLAVE_ADDR);
  
  // Function to run when data requested from master
  // Wire.onRequest(requestEvent); 
  
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);

  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Start Motor");
    Serial.println();
  }

  if (MOTORS_ENABLED) {
    AFMS.begin();
    // // setup motors
    // leftMotor.setupMotor();
    rightMotor.setupMotor();
  }
}

void loop() {
  if (MOTORS_ENABLED) {

    if (wireState == SIGNAL_STOP_EVERYTHING) {
      if (DEBUG) {
        Serial.println("Sending SIGNAL_STOP_EVERYTHING");
      }
      // leftMotor.stopEverything();
      rightMotor.stopEverything();
      // leftStepper->release();
      rightStepper->release();
      wireState = 0;
    }
    if (wireState == SIGNAL_INITIATE_BUTTON_HIGH) {
      // leftMotor.initiateAction(HIGH);
      if (DEBUG) {
        Serial.println("Sending SIGNAL_INITIATE_BUTTON_HIGH");
      }
      rightMotor.initiateAction(HIGH);
      wireState = 0;
    }

    // leftMotor.run();
    rightMotor.run();

    // TODO send motorState back to main to control sounds/leds
    // motorState1 = leftMotor.getState();
    motorState2 = rightMotor.getState();
  }

  if (DEBUG && MOTORS_ENABLED && printTime >= 1000) {
    printTime = 0;
    // leftMotor.report();
    rightMotor.report();
  } 

}
