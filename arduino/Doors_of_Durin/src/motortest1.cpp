// Using the Arduino AccelStepper Library
// https://hackaday.io/project/183279-accelstepper-the-missing-manual/details
// https://hackaday.io/project/183713/instructions



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

Adafruit_StepperMotor *leftStepperMotor = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *rightStepperMotor = AFMS.getStepper(200, 1);


void leftForwardStep() {
  leftStepperMotor->onestep(FORWARD, DOUBLE);
}
void leftBackwardStep() {
  leftStepperMotor->onestep(BACKWARD, DOUBLE);
}
void rightForwardStep() {
  rightStepperMotor->onestep(FORWARD, DOUBLE);
}

void rightBackwardstep() {
  rightStepperMotor->onestep(BACKWARD, DOUBLE);
}

AccelStepper leftStepper(leftForwardStep, leftBackwardStep);
AccelStepper rightStepper(rightForwardStep, rightBackwardstep);

elapsedMillis printTime;
const bool MOTORS_ENABLED = true;
const bool LEFT_ENABLED = false;
const bool RIGHT_ENABLED = true;
const bool DEBUG = true;
const int debounceTime = 200;
const int MAX_SPEED = 100;  // ~= 19 seconds per revolution


#define LEFT 0
#define RIGHT 1
int closeLimitSwitchPin[2] = {12, 11};
// int closeLimitSwitchPin[2] = {2, 3};
int closeLimitSwitchState[2] = {0, 0};
int lastCloseLimitSwitchState[2] = {-1, -1};
elapsedMillis lastCloseLimitSwitchTime[2];

int closingDirection[2] = {-1, 1};
int openingDirection[2] = {1, -1};
int openingMoveTarget[2] = {400, 400};
int motorAcceleration[2] = {200, 200};

const int goButtonPin = 10;
int goButtonState = 0;
int lastGoButtonState = -1;
elapsedMillis lastGoTime;

volatile int stopButtonState = 0;           // current state of the stop button (on interrupt)
int lastStopButtonState = -1;
elapsedMillis lastStopTime;

// UnoAccelStepperExper_3.ino
// define the time limit to run before stopping
const int timeLimit = 10; // number of seconds
// State definitions
// #define RSPD 01
// #define RJSTR 02
// #define RUN_HOME 03
// #define STOP_NOW 04

// State variable
volatile int motorState[2] = {MS_NOTHING, MS_NOTHING};
// volatile int state;   // must survive interrupts

void printName(int motorIndex) {
  if (motorIndex == LEFT) {
    Serial.print("LEFT: ");
  } else {
    Serial.print("RIGHT: ");
  }
}
void print(String val, int motorIndex = 0) {
  if (DEBUG) {
    Serial.print(val);
  }
}
void println(String val, int motorIndex = 0) {
  if (DEBUG) {
    Serial.println(val);
  }
}
void print(long val, int motorIndex = 0) {
  if (DEBUG) {
    Serial.print(val);
  }
}
void println(long val, int motorIndex = 0) {
  if (DEBUG) {
    Serial.println(val);
  }
}
boolean getStepperIsRunning(int motorIndex) {
  if (motorIndex == LEFT) {
    return leftStepper.isRunning();
  }
  if (motorIndex == RIGHT) {
    return rightStepper.isRunning();
  }
  return 0.0;
}
float getStepperSpeed(int motorIndex) {
  if (motorIndex == LEFT) {
    return leftStepper.speed();
  }
  if (motorIndex == RIGHT) {
    return rightStepper.speed();
  }
  return 0.0;
}
long getStepperCurrentPosition(int motorIndex) {
  if (motorIndex == LEFT) {
    return leftStepper.currentPosition();
  }
  if (motorIndex == RIGHT) {
    return rightStepper.currentPosition();
  }
  return 0l;
}
void printMotorState(int motorIndex) {
  if (DEBUG) {
      printName(motorIndex);
      print("State: ", motorIndex);
      switch (motorState[motorIndex]) {
        case MS_NOTHING:
          print("MS_NOTHING", motorIndex);
          break;
        case MS_CLOSING:
          print("MS_CLOSING", motorIndex);
          break;
        case MS_STOPPING:
          print("MS_STOPPING", motorIndex);
          break;
        case MS_STOPPED:
          print("MS_STOPPED", motorIndex);
          break;
        case MS_OPENING:
          print("MS_OPENING", motorIndex);
          break;
        case MS_FULLY_OPENED:
          print("MS_FULLY_OPENED", motorIndex);
          break;

        default:
          print("DEFAULT MS", motorIndex);
          break;
      }
      print(" (", motorIndex);
      print(motorState[motorIndex], motorIndex);
      print(") speed: ", motorIndex);
      print(getStepperSpeed(motorIndex), motorIndex);
      print(" position: ", motorIndex);
      println(getStepperCurrentPosition(motorIndex), motorIndex);
    }
}
boolean motorEnabled(int motorIndex) {
  if (motorIndex == LEFT) {
    return LEFT_ENABLED;
  }
  if (motorIndex == RIGHT) {
    return RIGHT_ENABLED;
  }
  return false;
}

void stopMotor(int motorIndex) {
  if (motorEnabled(LEFT)) {
    leftStepper.setAcceleration(200.0);
    leftStepper.stop();
    leftStepper.runToPosition();
  }
  if (motorEnabled(RIGHT)) {
    rightStepper.setAcceleration(200.0);
    rightStepper.stop();
    rightStepper.runToPosition();
  }
}
void stepperRunSpeed(int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.runSpeed();
  } else {
    rightStepper.runSpeed();
  }
}
void stepperRunSpeedToPosition(int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.runSpeedToPosition();
  } else {
    rightStepper.runSpeedToPosition();
  }
}
void stepperRun(int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.run();
  } else {
    rightStepper.run();
  }
}
void stepperSetAcceleration(float acceleration, int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.setAcceleration(acceleration);
  } else {
    rightStepper.setAcceleration(acceleration);
  }
}void stepperMove(int relative, int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.move(relative);
  } else {
    rightStepper.move(relative);
  }
}
long stepperDistanceToGo(int motorIndex) {
  if (motorIndex == LEFT) {
    return leftStepper.distanceToGo();
  } else {
    return rightStepper.distanceToGo();
  }
}
void stepperMoveTo(int targetPosition, int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.moveTo(targetPosition);
  } else {
    rightStepper.moveTo(targetPosition);
  }
}
void stepperSetSpeed(float speed, int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.setSpeed(speed);
  } else {
    rightStepper.setSpeed(speed);
  }
}
// Useful during initialisations or after initial positioning
// Sets speed to 0
void stepperSetCurrentPosition(long position, int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.setCurrentPosition(position);
  } else {
    rightStepper.setCurrentPosition(position);
  }
}

// Interrupt when the STOP button is pressed
// Set state on all motors to STOP
void stopEverything() {
  println("STOP!!!!!");


  for (int motorIndex=0; motorIndex<2; motorIndex++) {
    motorState[motorIndex] = MS_NOTHING;
    stopMotor(motorIndex);
  }

}
void checkStopButton() {
  if ( lastStopTime >= debounceTime) {
    stopButtonState = digitalRead(DOD_PIN_stopButton);
    // if (stopButtonState != lastStopButtonState) {
      print("STOP button: ");
      if (stopButtonState == HIGH) {
        println("HIGH - PRESSED");
        stopEverything();
      } else {
        println("LOW - NOT PRESSED");
      }
      lastStopButtonState = stopButtonState;
    // }
    // lastStopTime = 0;
  }
}

void initiateAction(int motorIndex) {
  if (!motorEnabled(motorIndex)) {
    return;
  }

  // Check current state
  switch(motorState[motorIndex]) {
    case MS_NOTHING:
      motorState[motorIndex] = MS_CLOSING;  // 0 to 2 ---  motorState[motorIndex] += 2;
      break;
    case MS_STOPPED: // limit switch is closed
      motorState[motorIndex] = MS_OPENING;  // 4 to 6 ---  motorState[motorIndex] += 2;
      break;
    case MS_OPENING:
      stepperSetAcceleration(motorAcceleration[motorIndex], motorIndex);
      stepperMove(openingDirection[motorIndex] * openingMoveTarget[motorIndex], motorIndex);
      stepperSetSpeed(openingDirection[motorIndex] * MAX_SPEED, motorIndex);
      break;
    case MS_FULLY_OPENED: // doors open to widest position - can only close now
      motorState[motorIndex] = MS_CLOSING;  // 8 to 2
      break;
    default:
      break;
  }
}

void checkGoButton() {
  if ( lastGoTime >= debounceTime) {
    goButtonState = digitalRead(goButtonPin);
    if (goButtonState != lastGoButtonState) {
      print("GO button: ");
      if (goButtonState == HIGH) {
        println("HIGH - PRESSED");
        // if (DEBUG) {
        //   Serial.print("goButtonState PRESSED ");
        //   Serial.println(goButtonState);
        // }
        // initiateAction(LEFT);
        // initiateAction(RIGHT);
        for (int motorIndex=0; motorIndex<2; motorIndex++) {
          initiateAction(motorIndex);
        }
      } else {
        println("LOW - NOT PRESSED");
      }
      lastGoButtonState = goButtonState;
    }
    lastGoTime = 0;
  }
}

void checkLimitSwitchClosed(int motorIndex) {
  if ( lastCloseLimitSwitchTime[motorIndex] >= debounceTime) {
    closeLimitSwitchState[motorIndex] = digitalRead(closeLimitSwitchPin[motorIndex]);

    if (closeLimitSwitchState[motorIndex] != lastCloseLimitSwitchState[motorIndex]) {
      
      printName(motorIndex);
      print("LIMIT switch: ", motorIndex);
      
      if (closeLimitSwitchState[motorIndex] == LOW) { // LOW = Closed
        println("LOW - CLOSED", motorIndex);

        // state = STOP_NOW;
        motorState[motorIndex] = MS_STOPPING;

        // if closing or closed already
        // if (motorState[motorIndex] < MOTOR_STATE_STOP) {
        //   motorState[motorIndex] = MOTOR_STATE_STOP;
        // }
      // } else {
        // println("HIGH - OPEN", motorIndex);
      }
      lastCloseLimitSwitchState[motorIndex] = closeLimitSwitchState[motorIndex];
    }
    lastCloseLimitSwitchTime[motorIndex] = 0;
  }
}

void motor_setup() {
  if (MOTORS_ENABLED) {
    AFMS.begin();

    // pinMode(closeLimitSwitchPin[RIGHT], INPUT_PULLUP);
    // checkLimitSwitchClosed(RIGHT);
    if (motorEnabled(LEFT)) {
      motorState[LEFT] = MS_NOTHING;
      pinMode(closeLimitSwitchPin[LEFT], INPUT_PULLUP);

      checkLimitSwitchClosed(LEFT);
      // motorState[LEFT] = RJSTR;   // initial state is run, just run
      leftStepper.setMaxSpeed(200.0);
      leftStepper.setAcceleration(50.0);
      // leftStepper.moveTo(-10000);
    }
    if (motorEnabled(RIGHT)) {
      motorState[RIGHT] = MS_NOTHING;
      pinMode(closeLimitSwitchPin[RIGHT], INPUT_PULLUP);

      checkLimitSwitchClosed(RIGHT);
      // motorState[RIGHT] = RJSTR;   // initial state is run, just run
      rightStepper.setMaxSpeed(200.0);
      rightStepper.setAcceleration(50.0);
      // rightStepper.moveTo(10000);
    }


    delay(500); // avoids startup pause
    Serial.println("motor_setup done");
    //   delay(1500);  // avoids startup pause
  }
}


void setup() {

  //   delay( 2000 ); // power-up safety delay
  //   delay( 2000 ); // power-up safety delay

  if (DEBUG) {
    // Serial.begin(9600);
    Serial.begin(115200);
    Serial.println("Start");
    Serial.println();
  }

// Setup Emergency STOP button  
  pinMode(DOD_PIN_stopButton, INPUT);
  attachInterrupt(digitalPinToInterrupt(DOD_PIN_stopButton), checkStopButton, RISING);

  motor_setup();
}



// Run the motors based on the state
// int lCount = 0; // elapsed seconds
// int simulatedOpeningCount = 0; // elapsed seconds
void motor_loop(int motorIndex) {

  if (printTime >= 1000) { // reports speed and position each second
      printTime = 0;
      printMotorState(motorIndex);
  }

  switch (motorState[motorIndex]) {
    case MS_CLOSING:
      stepperSetSpeed(125.0, motorIndex);
      stepperRun(motorIndex);
      break;
    case MS_STOPPING:
      stopMotor(motorIndex);
      if (getStepperIsRunning(motorIndex) != true) {
        stepperSetCurrentPosition(0, motorIndex);
        motorState[motorIndex] = MS_STOPPED;
      }
      break;
    case MS_OPENING:
      stepperRun(motorIndex);
      if (getStepperCurrentPosition(motorIndex) == openingDirection[motorIndex] * openingMoveTarget[motorIndex]) {
        if (getStepperIsRunning(motorIndex) != true) {
          motorState[motorIndex] = MS_FULLY_OPENED;
        }
      }
      break;
    default:
      break;
  }


// switch (motorState[motorIndex]) {
//   case MS_OPENING:
//     openingCount++;
//     break;
//   default:
//     openingCount = 0;
//     break;
// }
  // Set motor states
  // float mSpeed;
  // if (printTime >= 1000) { // reports speed and position each second
  //   printTime = 0;
  //   // lCount++;
  //   mSpeed = getStepperSpeed(motorIndex);
  //   switch (motorState[motorIndex]) {
  //     case RSPD:
  //       // if (lCount >= timeLimit) {
  //         digitalWrite(closeLimitSwitchPin[motorIndex], LOW);    // This will trigger interrupt
  //         // digitalWrite(closeLimitSwitchPin[RIGHT], LOW);    // This will trigger interrupt
  //         // digitalWrite(sensorPin,HIGH);    // This will trigger interrupt
  //       // }
  //       break;
  //     case RJSTR:
  //       if (mSpeed <= -200.0) {
  //         motorState[motorIndex] = RSPD;   // switch to run speed state when target speed is reached
  //         // state = RSPD;   // switch to run speed state when target speed is reached
  //       }
  //       break;
  //     case RUN_HOME:
  //     case STOP_NOW:
  //       break;
  //   }
  // }



  // control motors
  // switch (motorState[motorIndex]) {    // happens each loop - about 70KHz
  //   case RSPD:
  //     stepperRunSpeed(motorIndex);
  //     break;
  //   case RJSTR:
  //   case RUN_HOME:
  //     stepperRun(motorIndex);
  //     break;
  //   case STOP_NOW:
  //     digitalWrite(closeLimitSwitchPin[motorIndex], HIGH);    // removes interrupt signal

  //     if (motorIndex == LEFT) {
  //       leftStepper.setAcceleration(200.0);  // this makes motor stop much quicker!
  //       leftStepper.stop();
  //       leftStepper.runToPosition();  // brings to a stop!
  //     }
  //     if (motorIndex == RIGHT) {
  //       rightStepper.setAcceleration(200.0);  // this makes motor stop much quicker!
  //       rightStepper.stop();
  //       rightStepper.runToPosition();  // brings to a stop!
  //     }

  //     motorState[motorIndex] = RUN_HOME;
  //     break;
  // }
}

void loop() {
  for (int motorIndex=0; motorIndex<2; motorIndex++) {
    
    checkLimitSwitchClosed(motorIndex);

    if (motorEnabled(motorIndex)) {
      motor_loop(motorIndex);
    }
  }

  checkGoButton();    // sets new state.  WIll run motos with new state on next loop in motor_loop

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