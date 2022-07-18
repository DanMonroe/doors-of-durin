/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/

#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

#include "GatesOfMoria_Motor.h"

const uint8_t stopButtonPin = 2; // Button to perform stop everything interrupt
volatile int stopButtonState = 0;     // current state of the stop button
volatile uint8_t lastStopButtonState = 0;
uint8_t stopDebounceTime = 50;

const bool DEBUG = true;

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

// AccelStepper GOM_Astepper1(forwardstep1, backwardstep1);
AccelStepper GOM_Astepper2(forwardstep2, backwardstep2);

// GatesOfMoria_Motor::GatesOfMoria_Motor(AccelStepper _stepper, uint8_t _closedLimitSwitchPin, uint8_t _moveButtonPin, uint8_t _directionTogglePin, uint8_t _motorRunningLEDPin) {
GatesOfMoria_Motor motor2 = GatesOfMoria_Motor(DEBUG, GOM_Astepper2, 8, 9, 4, 7);


elapsedMillis printTime;
elapsedMillis lastStopButtonCheckTime;

void setup() {
  
  if (DEBUG) {
    Serial.begin(115200);
    // Serial.begin(9600);
    Serial.println("Start 11");
  }
  
  pinMode(stopButtonPin, INPUT);
  // pinMode(stopButtonPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(stopButtonPin), checkStopButton, CHANGE);
  // attachInterrupt(stopButtonPin, stopEverything, FALLING);
  
  AFMS.begin();

  // // setup motors
  // // motor1.setupMotor();
  motor2.setupMotor();
}


void loop() {
  // checkStopButton();

  // motor1.run();
  motor2.run();

  // if (printTime >= 5000) {
  //   printTime = 0;
  //   // motor1.report("Motor 1");
  //   motor2.report("Motor 2");
  // } 

}

void checkStopButton() {
  // Serial.println("Check stop");
  if ( lastStopButtonCheckTime >= stopDebounceTime ) {
    stopButtonState = digitalRead(stopButtonPin);
    if (stopButtonState != lastStopButtonState) {
      if (stopButtonState == HIGH) {
        stopEverything();
      }
    }
    lastStopButtonCheckTime = 0;
    lastStopButtonState = stopButtonState;
  }
}

// Interrupt when the STOP button is pressed
// Set state on all motors to STOP
void stopEverything() {
  Serial.println("STOP!");
  // motor1.stopEverything("Motor 1");
  motor2.stopEverything("Motor 2");
  // if (enabFlag == 1) {
  //   state = STOP_NOW;
  //   enabFlag = 0;
  // }
}
