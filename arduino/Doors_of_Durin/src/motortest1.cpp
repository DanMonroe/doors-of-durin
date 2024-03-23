#include <Arduino.h>
#include "pins.h"
#include <Wire.h>
#include <elapsedMillis.h>
#include <SPI.h>
#include <SD.h>
#include "motorStates.h"
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Adafruit_StepperMotor *leftStepperMotor = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *rightStepperMotor = AFMS.getStepper(200, 1); // working

void rightForwardStep() {
  rightStepperMotor->onestep(FORWARD, DOUBLE);
}

void rightBackwardstep() {
  rightStepperMotor->onestep(BACKWARD, DOUBLE);
}

AccelStepper rightStepper(rightForwardStep, rightBackwardstep);

elapsedMillis printTime;
const bool DEBUG = true;
const int debounceTime = 200;

#define LEFT 0
#define RIGHT 1
int closeLimitSwitchPin[2] = {12, 11};
// int closeLimitSwitchPin[2] = {2, 3};
int closeLimitSwitchState[2] = {0, 0};
int lastCloseLimitSwitchState[2] = {-1, -1};
elapsedMillis lastCloseLimitSwitchTime[2];

// UnoAccelStepperExper_3.ino
// define the time limit to run before stopping
const int timeLimit = 10; // number of seconds
// State definitions
#define RSPD 01
#define RJSTR 02
#define RUN_HOME 03
#define STOP_NOW 04
// State variable
volatile int state;   // must survive interrupts

void printName(int motorIndex) {
  if (motorIndex == LEFT) {
    Serial.print("LEFT: ");
  } else {
    Serial.print("RIGHT: ");
  }
}
void print(String val, int motorIndex) {
  if (DEBUG) {
    Serial.print(val);
  }
}
void println(String val, int motorIndex) {
  if (DEBUG) {
    Serial.println(val);
  }
}
void print(long val, int motorIndex) {
  if (DEBUG) {
    Serial.print(val);
  }
}
void println(long val, int motorIndex) {
  if (DEBUG) {
    Serial.println(val);
  }
}

void checkLimitSwitchClosed(int motorIndex) {
  closeLimitSwitchState[motorIndex] = digitalRead(closeLimitSwitchPin[motorIndex]);

  // Serial.print("Left Switch ")
  // Serial.print(closeLimitSwitchState[motorIndex]);
  // Serial.println(lastCloseLimitSwitchState[motorIndex]);

  if (closeLimitSwitchState[motorIndex] != lastCloseLimitSwitchState[motorIndex]) {
    
    printName(motorIndex);
    print("LIMIT switch: ", motorIndex);
    
    if (closeLimitSwitchState[motorIndex] == LOW) { // LOW = Closed
      println("LOW", motorIndex);

      state = STOP_NOW;

      // if closing or closed already
      // if (motorState[motorIndex] < MOTOR_STATE_STOP) {
      //   motorState[motorIndex] = MOTOR_STATE_STOP;
      // }
    } else {
      println("HIGH", motorIndex);
    }
    lastCloseLimitSwitchState[motorIndex] = closeLimitSwitchState[motorIndex];
  }
}

void motor_setup() {
  AFMS.begin();

  pinMode(closeLimitSwitchPin[RIGHT], INPUT_PULLUP);

  checkLimitSwitchClosed(RIGHT);

  state = RJSTR;   // initial state is run, just run
  // motorState[RIGHT] = MOTOR_STATE_NOTHING;

  rightStepper.setMaxSpeed(400.0);
  rightStepper.setAcceleration(50.0);
  rightStepper.moveTo(10000);
  // rightStepper.setSpeed(50);

  delay(500); // avoids startup pause
  Serial.println("motor_setup done");
  //   delay(1500);  // avoids startup pause
}


void setup() {

  //   delay( 2000 ); // power-up safety delay
  //   delay( 2000 ); // power-up safety delay

  if (DEBUG) {
    Serial.begin(9600);
    // Serial.begin(19200);
    // Serial.begin(115200);
    Serial.println("Start");
    Serial.println();
  }

  motor_setup();
}

int lCount = 0; // elapsed seconds
void motor_loop() {
  // rightStepper.runSpeed();
  // if (!rightStepper.run()) {
  //     rightStepper.moveTo(-rightStepper.currentPosition());
  // }

  float mSpeed;
  if (printTime >= 1000) { // reports speed and position each second
    printTime = 0;
    lCount++;
    mSpeed = rightStepper.speed();
    Serial.print("  "); //////
    Serial.print(mSpeed);
    Serial.print("  ");
    Serial.println(rightStepper.currentPosition());
  switch (state) {
      case RSPD:
        if (lCount >= timeLimit) {
          digitalWrite(closeLimitSwitchPin[RIGHT], LOW);    // This will trigger interrupt
          // digitalWrite(sensorPin,HIGH);    // This will trigger interrupt
        }
        break;
      case RJSTR:
        if (mSpeed <= -200.0) {
          state = RSPD;   // switch to run speed state when target speed is reached
        }
        break;
      case RUN_HOME:
      case STOP_NOW:
        break;
    }
  }
  switch (state) {    // happens each loop - about 70KHz
    case RSPD:
      rightStepper.runSpeed();
      break;
    case RJSTR:
    case RUN_HOME:
      rightStepper.run();
      break;
    case STOP_NOW:
      digitalWrite(closeLimitSwitchPin[RIGHT], HIGH);    // removes interrupt signal
      // digitalWrite(sensorPin,LOW);    // removes interrupt signal
      rightStepper.setAcceleration(200.0);  // this makes motor stop much quicker!
      rightStepper.stop();
      rightStepper.runToPosition();  // brings to a stop!
      rightStepper.moveTo(0);  // now return to home position
      rightStepper.setAcceleration(50.0);  // slow motor acceleration back down
      //rightStepper.move(-rightStepper.currentPosition()); // This should work also
      state = RUN_HOME;
      break;
  }
  //   // if (mSpeed <= -200.0) {       // for negative rotation
  //   if (mSpeed >= 200.0) {               // for positive rotation
  //     state = RSPD; // switch to run speed state when target speed is reached
  //   }
  //   switch (state) {
  //     case RSPD:
  //       if (lCount >= timeLimit) {
  //         rightStepper.setAcceleration(200.0); // this makes motor stop much quicker!
  //         rightStepper.stop();
  //         rightStepper.runToPosition(); // go immediately to stop position!
  //         state = STOPPED;
  //       }
  //       break;
  //     case RJSTR:
  //       if (mSpeed >= 200.0) {
  //         state = RSPD; // switch to run speed state when target speed is reached
  //       }
  //       break;
  //     case STOPPED:
  //       break;
  //   }
  // }
  // switch (state) {
  //   case RSPD:
  //     rightStepper.runSpeed();
  //     break;
  //   case RJSTR:
  //     rightStepper.run();
  //     break;
  //   case STOPPED:
  //     break;
  // }
  // switch (state) {
  //   case RSPD:
  //     rightStepper.runSpeed();
  //     break;
  //   case RJSTR:
  //     rightStepper.run();
  //     break;
  // }
}

void loop() {
  for (int motorIndex=0; motorIndex<2; motorIndex++) {
    if ( lastCloseLimitSwitchTime[motorIndex] >= debounceTime) {

      // printName(motorIndex);
      // print('close limit state: ', motorIndex);
      // print(closeLimitSwitchState[motorIndex], motorIndex);
      // print(' last state: ', motorIndex);
      // println(lastCloseLimitSwitchState[motorIndex], motorIndex);

      checkLimitSwitchClosed(motorIndex);
      lastCloseLimitSwitchTime[motorIndex] = 0;
    }
  }
  motor_loop();
}

// void setup() {
//   Serial.begin(115200);
//   // set the maximum speed, acceleration factor, and the target position.
//   rightStepperMotor.setMaxSpeed(200.0);   // the motor accelerates to this speed exactly without overshoot. Try other values.
//   rightStepperMotor.setAcceleration(50.0);   // try other acceleration rates.
//   rightStepperMotor.moveTo(1500);
//   // For negative rotation
//   //rightStepperMotor.moveTo(-10000);    // This will demonstrate a move in the negative direction.
// }
// int count = 0;    // tracks seconds to trigger an action if desired.
// void loop() {
//   float mSpeed;
//   if (printTime >= 1000) {    // reports speed and position each second
//     printTime = 0;
//     mSpeed = rightStepperMotor.speed();
//     Serial.print("  ");  //////
//     Serial.print(mSpeed);
//     Serial.print("  ");
//     Serial.print(rightStepperMotor.currentPosition());
//     Serial.print("  ");
//     Serial.println(count);
//     // uncomment an action to see the response
//     //   in the serial monitor
//     if (count++ == 4){
//       //rightStepperMotor.moveTo(100);
//       //rightStepperMotor.move(100);
//       rightStepperMotor.setAcceleration(200.0);
//       rightStepperMotor.stop();
//     }
//     if (count == 20) while(1);  ///////
//   }
//   rightStepperMotor.run();
// }