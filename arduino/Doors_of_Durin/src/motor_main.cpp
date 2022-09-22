#include <Arduino.h>

// #include "pins.h"
#include "motorStates.h"
// #include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

const bool MOTORS_ENABLED = true;
const bool LEFT_ENABLED = true;
const bool RIGHT_ENABLED = true;
const bool DEBUG = true;

unsigned int REPORT_TIME = 2500;

int wireState = 0;
int lastWireState = -1;

elapsedMillis printTime;

// define Slave I2C address
#define SLAVE_ADDR 9
#define SIGNAL_INITIATE_BUTTON_HIGH 200

#define LEFT 0
#define RIGHT 1

const int MAX_SPEED = 100;  // ~= 19 seconds per revolution
const int debounceTime = 200;

int motorState[2] = {MOTOR_STATE_NOTHING, MOTOR_STATE_NOTHING};
int motorAcceleration[2] = {200, 200};
int closeLimitSwitchPin[2] = {2, 3}; 
int closeLimitSwitchState[2] = {0, 0};
int lastCloseLimitSwitchState[2] = {-1, -1};
elapsedMillis lastCloseLimitSwitchTime[2];
elapsedMillis rampUpTime[2];
int closingDirection[2] = {-1, 1};
int openingDirection[2] = {1, -1};

// const int initiateActionButtonPin = 10;
// int initiateActionButtonState = 0;
// int lastInitiateActionButtonState = -1;
// elapsedMillis lastInitiateActionTime;


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

Adafruit_StepperMotor *leftStepperMotor = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *rightStepperMotor = AFMS.getStepper(200, 1);  // working

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

void stepperSetAcceleration(float acceleration, int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.setAcceleration(acceleration);
  } else {
    rightStepper.setAcceleration(acceleration);
  }
}
void stepperSetSpeed(int speed, int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.setSpeed(speed);
  } else {
    rightStepper.setSpeed(speed);
  }
}
void stepperMove(int relative, int motorIndex) {
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
void stepperRun(int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.run();
  } else {
    rightStepper.run();
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
void stepperSetCurrentPosition(int position, int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.setCurrentPosition(position);
  } else {
    rightStepper.setCurrentPosition(position);
  }
}
void stepperStop(int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.stop();
  } else {
    rightStepper.stop();
  }
}
void stepperDisableOutputs(int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.disableOutputs();
  } else {
    rightStepper.disableOutputs();
  }
}
void stepperEnableOutputs(int motorIndex) {
  if (motorIndex == LEFT) {
    leftStepper.enableOutputs();
  } else {
    rightStepper.enableOutputs();
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

void initiateAction(int motorIndex) {
  if (!motorEnabled(motorIndex)) {
    return;
  }

  motorState[motorIndex]++;
  switch(motorState[motorIndex]) {
    case MOTOR_STATE_RUN_CLOSING:
      if (motorEnabled(motorIndex)) {
        stepperEnableOutputs(motorIndex);
        stepperSetAcceleration(motorAcceleration[motorIndex], motorIndex);
        stepperMove(closingDirection[motorIndex]*2000, motorIndex);
        stepperSetSpeed(closingDirection[motorIndex]*MAX_SPEED, motorIndex);
      }
      break;
    case MOTOR_STATE_RUN_OPENING:
      if (motorEnabled(motorIndex)) {
        stepperSetAcceleration(motorAcceleration[motorIndex], motorIndex);
        stepperMove(openingDirection[motorIndex]*800, motorIndex);
        stepperSetSpeed(openingDirection[motorIndex]*MAX_SPEED, motorIndex);

        // motorState[motorIndex] = 4; //  same as switch case value? MOTOR_STATE_RUN; // run
        // if (DEBUG) {
        //   Serial.println("");
        //   Serial.print("Opening ");
        //   Serial.println(motorIndex);
        //   Serial.println("");
        // }
      }
      break;

    default:
      break;
  }
}

void checkLimitSwitchClosed(int motorIndex) {
  closeLimitSwitchState[motorIndex] = digitalRead(closeLimitSwitchPin[motorIndex]);
  if (closeLimitSwitchState[motorIndex] != lastCloseLimitSwitchState[motorIndex]) {
    if (closeLimitSwitchState[motorIndex] == LOW) {
      // if closing or closed already
      if (motorState[motorIndex] < MOTOR_STATE_STOP) {
        motorState[motorIndex] = MOTOR_STATE_STOP;
      }
    }
    lastCloseLimitSwitchState[motorIndex] = closeLimitSwitchState[motorIndex];
  }
}

void setup() {

  // Initialize I2C communications as Slave
  Wire.begin(SLAVE_ADDR);
  
  // Function to run when data requested from master
  // Wire.onRequest(requestEvent); 
  
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);

  // if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Start Motor");
    Serial.println();
  // }

    AFMS.begin();

    if (motorEnabled(LEFT)) {
      motorState[LEFT] = MOTOR_STATE_NOTHING;
      pinMode(closeLimitSwitchPin[LEFT], INPUT_PULLUP);

      checkLimitSwitchClosed(LEFT);
    }
    if (motorEnabled(RIGHT)) {
      motorState[RIGHT] = MOTOR_STATE_NOTHING;
      pinMode(closeLimitSwitchPin[RIGHT], INPUT_PULLUP);

      checkLimitSwitchClosed(RIGHT);
    }

    delay(1500);  // avoids startup pause
}

void loop() {

  if (wireState != lastWireState) {
    if (wireState == SIGNAL_INITIATE_BUTTON_HIGH) {
      if (DEBUG) {
        Serial.println("initiate Action through I2C");
      }

      initiateAction(LEFT);
      initiateAction(RIGHT);
      wireState = 0;
    }
    lastWireState = wireState;
  }

  // if ( lastInitiateActionTime >= debounceTime) {
  //   initiateActionButtonState = digitalRead(initiateActionButtonPin);
  //   if (initiateActionButtonState != lastInitiateActionButtonState) {
  //     if (initiateActionButtonState == HIGH) {
  //       // if (DEBUG) {
  //       //   Serial.print("initiateActionButtonState PRESSED ");
  //       //   Serial.println(initiateActionButtonState);
  //       // }
  //       initiateAction(LEFT);
  //       initiateAction(RIGHT);
  //     }
  //     lastInitiateActionButtonState = initiateActionButtonState;
  //   }
  //   lastInitiateActionTime = 0;
  // }

  for (int motorIndex=0; motorIndex<MOTOR_STATE_RUNSPEED_CLOSING; motorIndex++) {
    if (motorEnabled(motorIndex)) {
      if ( lastCloseLimitSwitchTime[motorIndex] >= debounceTime) {
        checkLimitSwitchClosed(motorIndex);
        lastCloseLimitSwitchTime[motorIndex] = 0;
      }

      float mSpeed;
      // rampUp
      if (rampUpTime[motorIndex] >= 500) {
        rampUpTime[motorIndex] = 0;
        mSpeed = rightStepper.speed();
        // if (DEBUG) {
        //   Serial.print(mSpeed);
        //   Serial.print("  ");
        //   Serial.print(rightStepper.currentPosition());
        //   Serial.print("  ");
        //   Serial.print(motorState);
        //   Serial.print("  ");
        //   Serial.println(rightStepper.distanceToGo());
        // }
        if (motorState[motorIndex] == MOTOR_STATE_RUN_CLOSING) {
          if (mSpeed >= 200.0) {      // for positive rotation
            motorState[motorIndex] = MOTOR_STATE_RUNSPEED_CLOSING;   // switch to run speed state when target speed is reached
          }
        }
        if (motorState[motorIndex] == MOTOR_STATE_RUN_OPENING) {
          if (mSpeed <= -200.0) {       // for negative rotation
            motorState[motorIndex] = MOTOR_STATE_RUNSPEED_OPENING;   // switch to run speed state when target speed is reached
          }
        }
      }

      switch (motorState[motorIndex]) {
        case MOTOR_STATE_NOTHING:
          stepperDisableOutputs(motorIndex);
          break;
        case MOTOR_STATE_RUN_CLOSING:
          // Closing start
          stepperRun(motorIndex);
          break;
        case MOTOR_STATE_RUNSPEED_CLOSING:
          // Closing 
          stepperRunSpeed(motorIndex);
          break;
        case MOTOR_STATE_STOP:
          // Stopped
          // dont call run
          stepperSetCurrentPosition(0, motorIndex);
          break;
        case MOTOR_STATE_RUN_OPENING:
          // Opening start
          stepperRun(motorIndex);
          break;
        case MOTOR_STATE_RUNSPEED_OPENING:
          // Opening
          stepperRunSpeedToPosition(motorIndex);
          if (stepperDistanceToGo(motorIndex) == 0) {
            motorState[motorIndex] = MOTOR_STATE_NOTHING;
          }
          break;
        
        default:
          motorState[motorIndex] = MOTOR_STATE_NOTHING;
          break;
      }
    }
  } // for motorIndex

}

