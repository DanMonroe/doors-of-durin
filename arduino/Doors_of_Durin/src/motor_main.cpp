#include <Arduino.h>

#include "pins.h"
#include "motorStates.h"
// #include <DOD_PixelStrips.h>
// #include <Wire.h>
// #include <DOD_Motor.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

const bool MOTORS_ENABLED = true;
const bool LEFT_ENABLED = false;
const bool RIGHT_ENABLED = true;
const bool DEBUG = false;

unsigned int REPORT_TIME = 2500;

// int leftMotorState = 0;
// int rightMotorState = 0;
int wireState = 0;
int lastWireState = 0;

elapsedMillis printTime;

#define LEFT 0
#define RIGHT 1

const int MAX_SPEED = 100;  // ~= 19 seconds per revolution
const int debounceTime = 200;

int motorState[2] = {0, 0};
int motorAcceleration[2] = {200, 200};
int closeLimitSwitchPin[2] = {2, 3}; 
int closeLimitSwitchState[2] = {0, 0};
int lastCloseLimitSwitchState[2] = {-1, -1};
elapsedMillis lastCloseLimitSwitchTime[2];
elapsedMillis rampUpTime[2];
int closingDirection[2] = {-1, 1};
int openingDirection[2] = {1, -1};
    // bool initialized[2] = {false, false};
    // int currentDirection[2] = {0, 0};
    // int currentSpeed[2] = {0, 0};
    // long targetClosePosition[2] = {-2500, -2500};
    // long targetOpenPosition[2] = {300, 300};
    // int currentPosition[2] = {0, 0};

const int initiateActionButtonPin = 10;
int initiateActionButtonState = 0;
int lastInitiateActionButtonState = -1;
elapsedMillis lastInitiateActionTime;


// void receiveEvent(int signalCode) {
//   // Read while data received
//   while (0 < Wire.available()) {
//     wireState = Wire.read();
//     if (DEBUG) {
//       Serial.print("receiveEvent wireState ");
//       Serial.println(wireState);
//     }
//   }
// }
 
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
// void report(int motorIndex) {
//     float mSpeed = 0.0;
//     long curPosition = 0;
//     if (motorIndex == LEFT) {
//       mSpeed = leftStepper.speed();
//       curPosition = leftStepper.currentPosition();
//     } else {
//       mSpeed = rightStepper.speed();
//       curPosition = rightStepper.currentPosition();
//     }
//     printName(motorIndex);
//     print(" state: ", motorIndex);
//     switch(motorState[motorIndex]) {
//       case MOTOR_STATE_INIT:
//         println("INIT", motorIndex);
//         break;
//       case MOTOR_STATE_CLOSING:
//         println("CLOSING", motorIndex);
//         break;
//       case MOTOR_STATE_CLOSED:
//         println("CLOSED", motorIndex);
//         break;
//       case MOTOR_STATE_RUNSPEED:
//         println("RUNSPEED", motorIndex);
//         break;
//       case MOTOR_STATE_RUN:
//         println("RUN", motorIndex);
//         break;
//       case MOTOR_STATE_RUN_HOME:
//         println("RUN_HOME", motorIndex);
//         break;
//       case MOTOR_STATE_STOP:
//         println("STOP", motorIndex);
//         break;
//       case MOTOR_STATE_STOP_NOW:
//         println("STOP_NOW", motorIndex);
//         break;
//       case MOTOR_STATE_STOP_BY_CLOSE_LIMIT:
//         println("STOP_BY_CLOSE_LIMIT", motorIndex);
//         break;
//       case MOTOR_STATE_OPENING:
//         println("OPENING", motorIndex);
//         break;
//       case MOTOR_STATE_OPEN:
//         println("OPEN", motorIndex);
//         break;
//       default:
//         println("UNKNOWN!!!!!", motorIndex);
//         break;
//     }
//     print(" spd: ", motorIndex);
//     print(mSpeed, motorIndex);
//     print(" pos: ", motorIndex);
//     print(curPosition, motorIndex);
//     print(" trgt: ", motorIndex);
//     println(targetOpenPosition[motorIndex], motorIndex);
//     println("-----------------------------------------------------", motorIndex);
// }

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
    case 1:
      if (motorEnabled(motorIndex)) {
        stepperEnableOutputs(motorIndex);
        stepperSetAcceleration(motorAcceleration[motorIndex], motorIndex);
        stepperMove(closingDirection[motorIndex]*2000, motorIndex);
        stepperSetSpeed(closingDirection[motorIndex]*MAX_SPEED, motorIndex);

        // rightStepper.enableOutputs();
        // rightStepper.setAcceleration(200);
        // rightStepper.move(2000);
        // rightStepper.setSpeed(MAX_SPEED);
      }
      break;
    case 4: // stopped
      if (motorEnabled(motorIndex)) {
        stepperSetAcceleration(motorAcceleration[motorIndex], motorIndex);
        stepperMove(openingDirection[motorIndex]*800, motorIndex);
        stepperSetSpeed(openingDirection[motorIndex]*MAX_SPEED, motorIndex);

        // rightStepper.move(-800);
        // rightStepper.setSpeed(-MAX_SPEED);
        motorState[motorIndex] = 4; // run
        if (DEBUG) {
          Serial.println("");
          Serial.print("Opening ");
          Serial.println(motorIndex);
          Serial.println("");
        }
      }
      break;

    default:
      break;
  }


  // if (RIGHT_ENABLED) {
  // if (!initialized[motorIndex]) {
  //   // println("Starting to close to reset");
  //   // Start to close the door
  //   moveTo(targetClosePosition[motorIndex], motorIndex);
  //   setSpeed(closingDirection[motorIndex] * MAX_SPEED, motorIndex);
  //   motorState[motorIndex] = MOTOR_STATE_CLOSING;
  // } else {
  //   if(motorState[motorIndex] == MOTOR_STATE_OPEN) {
  //     // Door at max open position.
  //     // put into CLOSING state and rely on limit switch to stop
  //     currentSpeed[motorIndex] = (closingDirection[motorIndex] * MAX_SPEED);
  //     moveTo(targetClosePosition[motorIndex], motorIndex);
  //     setSpeed(currentSpeed[motorIndex], motorIndex);
  //     motorState[motorIndex] = MOTOR_STATE_CLOSING;
  //   } else if (motorState[motorIndex] == MOTOR_STATE_CLOSED) {
  //     currentSpeed[motorIndex] = (openingDirection[motorIndex] * MAX_SPEED);
  //     // Must call setSpeed AFTER moveTo
  //     moveTo(targetOpenPosition[motorIndex], motorIndex);
  //     setSpeed(currentSpeed[motorIndex], motorIndex);
  //     motorState[motorIndex] = MOTOR_STATE_OPENING;
  //   }
  // }
}

// int setState(int motorIndex) {
//   if (!motorEnabled(motorIndex)) {
//     return MOTOR_STATE_NOTHING;
//   }

//   // Close Limit Switch
//   if ( lastCloseLimitSwitchTime[motorIndex] >= debounceTime) {
//     closeLimitSwitchState[motorIndex] = digitalRead(closeLimitSwitchPin[motorIndex]);
//     if (closeLimitSwitchState[motorIndex] != lastCloseLimitSwitchState[motorIndex]) {
//       if (closeLimitSwitchState[motorIndex] == LOW) {
//         print("Limit Switch Closed ", motorIndex);
//         println(closeLimitSwitchPin[motorIndex], motorIndex);
//         // If starting while the close limit switch is closed, set ready to open.
//         if (!initialized[motorIndex] || motorState[motorIndex] == MOTOR_STATE_CLOSING) {
//           println("Initialized", motorIndex);
//           initialized[motorIndex] = true;
//           currentDirection[motorIndex] = openingDirection[motorIndex];
//           motorState[motorIndex] = MOTOR_STATE_CLOSED;
//         } else {
//           motorState[motorIndex] = MOTOR_STATE_STOP_BY_CLOSE_LIMIT;
//         }
//       } else {
//         // print("Limit Switch Open ");
//         // println(closeLimitSwitchPin);
//       }
//       lastCloseLimitSwitchState[motorIndex] = closeLimitSwitchState[motorIndex];
//       lastCloseLimitSwitchTime[motorIndex] = 0;
//     }
//   }
//   return motorState[motorIndex];
// }

// void run(int motorIndex) {
//   if (!motorEnabled(motorIndex)) {
//     return;
//   }
//   int thisState = setState(motorIndex);
//   switch(thisState) {
//     case MOTOR_STATE_NOTHING:
//     case MOTOR_STATE_INIT:
//       break;
//     case MOTOR_STATE_RUNSPEED:
//     case MOTOR_STATE_CLOSING:
//       // runningLEDState = HIGH;
//       // stepper.runSpeed();
//       // moveTo(targetClosePosition[motorIndex], motorIndex);
//       stepperRun(motorIndex);
//       // stepperRunToPosition(motorIndex);
//       break;
//     case MOTOR_STATE_OPENING:
//       if (initialized[motorIndex]) {
//         // runningLEDState = HIGH;
//         stepperRunToPosition(motorIndex);
//         // stepper.runSpeedToPosition();
//       }
//       break;
//     case MOTOR_STATE_CLOSED:
//       currentDirection[motorIndex] = openingDirection[motorIndex];
//       setSpeed(0, motorIndex);
//       stepperStop(motorIndex);
//       stepperSetCurrentPosition(0, motorIndex);
//       break;
//     case MOTOR_STATE_OPEN:
//       currentDirection[motorIndex] = closingDirection[motorIndex];
//       setSpeed(0, motorIndex);
//       stepperStop(motorIndex);
//       break;
//     case MOTOR_STATE_STOP:
//       setSpeed(0, motorIndex);
//       stepperStop(motorIndex);
//       break;
//     case MOTOR_STATE_STOP_NOW:
//       setSpeed(0, motorIndex);
//       stepperStop(motorIndex);
//       stepperDisableOutputs(motorIndex);
//       break;
//   }
// }

void setup() {

  // Initialize I2C communications as Slave
  // Wire.begin(SLAVE_ADDR);
  
  // Function to run when data requested from master
  // Wire.onRequest(requestEvent); 
  
  // Function to run when data received from master
  // Wire.onReceive(receiveEvent);

  // if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Start Motor");
    Serial.println();
  // }

    AFMS.begin();

    if (motorEnabled(LEFT)) {
      motorState[LEFT] = 0;
      pinMode(closeLimitSwitchPin[LEFT], INPUT_PULLUP);
    }
    if (motorEnabled(RIGHT)) {
      motorState[RIGHT] = 0;
      pinMode(closeLimitSwitchPin[RIGHT], INPUT_PULLUP);
    }

    delay(1500);  // avoids startup pause

    // pinMode(DOD_PIN_initiateActionButtonPin, INPUT_PULLUP);

    // if (LEFT_ENABLED) {
    //   motorState[LEFT] = MOTOR_STATE_INIT;
    //   pinMode(closeLimitSwitchPin[LEFT], INPUT_PULLUP);
    //   closeLimitSwitchState[LEFT] = digitalRead(closeLimitSwitchPin[LEFT]);
    //   lastCloseLimitSwitchState[LEFT] = closeLimitSwitchState[LEFT];
    //   if (closeLimitSwitchState[LEFT] == LOW) {
    //     motorState[LEFT] = MOTOR_STATE_STOP_BY_CLOSE_LIMIT;
    //     if (!initialized[LEFT]) {
    //       initialized[LEFT] = true;
    //       currentDirection[LEFT] = openingDirection[LEFT];
    //       motorState[LEFT] = MOTOR_STATE_CLOSED;
    //     }
    //   }


    // if (RIGHT_ENABLED) {
    //   motorState[RIGHT] = MOTOR_STATE_INIT;
    //   pinMode(closeLimitSwitchPin[RIGHT], INPUT_PULLUP);
    //   closeLimitSwitchState[RIGHT] = digitalRead(closeLimitSwitchPin[RIGHT]);
    //   lastCloseLimitSwitchState[RIGHT] = closeLimitSwitchState[RIGHT];

    //   print("closeLimitSwitchState: ", RIGHT);
    //   println(closeLimitSwitchState[RIGHT], RIGHT);

    //   if (closeLimitSwitchState[RIGHT] == LOW) {
    //     motorState[RIGHT] = MOTOR_STATE_STOP_BY_CLOSE_LIMIT;
    //     if (!initialized[RIGHT]) {
    //       initialized[RIGHT] = true;
    //       currentDirection[RIGHT] = openingDirection[RIGHT];
    //       motorState[RIGHT] = MOTOR_STATE_CLOSED;
    //     }
    //   }
    //   // rightStepper.setMaxSpeed(MAX_SPEED);
    //   // // rightStepper.setAcceleration(100);
    //   // rightStepper.setSpeed(currentDirection[RIGHT] * MAX_SPEED);
    // }
  // }
}

void loop() {

    if ( lastInitiateActionTime >= debounceTime) {
    initiateActionButtonState = digitalRead(initiateActionButtonPin);
    if (initiateActionButtonState != lastInitiateActionButtonState) {
      if (initiateActionButtonState == HIGH) {
        if (DEBUG) {
          Serial.print("initiateActionButtonState PRESSED ");
          Serial.println(initiateActionButtonState);
        }
        initiateAction(LEFT);
        initiateAction(RIGHT);
      }
      lastInitiateActionButtonState = initiateActionButtonState;
    }
    lastInitiateActionTime = 0;
  }

  for (int motorIndex=0; motorIndex<2; motorIndex++) {
    if (motorEnabled(motorIndex)) {
      if ( lastCloseLimitSwitchTime[motorIndex] >= debounceTime) {
        closeLimitSwitchState[motorIndex] = digitalRead(closeLimitSwitchPin[motorIndex]);
        if (closeLimitSwitchState[motorIndex] != lastCloseLimitSwitchState[motorIndex]) {
          if (closeLimitSwitchState[motorIndex] == LOW) {
            // if closing or closed already
            if (motorState[motorIndex] < 3) {
            // if (motorState == 1 || motorState == 2) {
              motorState[motorIndex] = 3;
            }
          }
          lastCloseLimitSwitchState[motorIndex] = closeLimitSwitchState[motorIndex];
        }
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
        if (motorState[motorIndex] == 1) {
          if (mSpeed >= 200.0) {      // for positive rotation
            motorState[motorIndex] = 2;   // switch to run speed state when target speed is reached
          }
        }
        if (motorState[motorIndex] == 4) {
          if (mSpeed <= -200.0) {       // for negative rotation
            motorState[motorIndex] = 5;   // switch to run speed state when target speed is reached
          }
        }
      }

      switch (motorState[motorIndex]) {
        case 0:
          stepperDisableOutputs(motorIndex);
          break;
        case 1:
          // Closing start
          stepperRun(motorIndex);
          break;
        case 2:
          // Closing 
          stepperRunSpeed(motorIndex);
          break;
        case 3:
          // Stopped
          // dont call run
          stepperSetCurrentPosition(0, motorIndex);
          break;
        case 4:
          // Opening start
          stepperRun(motorIndex);
          break;
        case 5:
          // Opening
          stepperRunSpeedToPosition(motorIndex);
          if (stepperDistanceToGo(motorIndex) == 0) {
            motorState[motorIndex] = 0;
          }
          break;
        
        default:
          motorState[motorIndex] = 0;
          break;
      }
    }
  } // for motorIndex

  // if (MOTORS_ENABLED) {
  //   if ( lastInitiateActionTime >= debounceTime) {
  //     initiateActionButtonState = digitalRead(DOD_PIN_initiateActionButtonPin);
  //     if (initiateActionButtonState != lastInitiateActionButtonState) {
  //       if (DEBUG) {
  //         Serial.print("initiateActionButtonState ");
  //         Serial.print(initiateActionButtonState);
  //         Serial.println("    INITIATING!");
  //       }
  //       if (LEFT_ENABLED) {
  //         initiateAction(LEFT);
  //       }
  //       if (RIGHT_ENABLED) {
  //         initiateAction(RIGHT);
  //       }
  //       lastInitiateActionButtonState = initiateActionButtonState;
  //     }
  //   }
  //   // if (wireState == SIGNAL_STOP_EVERYTHING) {
  //   //   if (DEBUG) {
  //   //     Serial.println("Sending SIGNAL_STOP_EVERYTHING");
  //   //   }
  //   //   if (LEFT_ENABLED) {
  //   //     // leftMotor.stopEverything();
  //   //     // leftStepper->release();
  //   //   }
  //   //   if (RIGHT_ENABLED) {
  //   //     // rightMotor.stopEverything();
  //   //     // rightStepper.release();
  //   //   }
  //   //   wireState = 0;
  //   // }
  //   // if (wireState == SIGNAL_INITIATE_BUTTON_HIGH) {
  //   //   if (DEBUG) {
  //   //     Serial.println("Sending SIGNAL_INITIATE_BUTTON_HIGH");
  //   //   }
  //   //   // if (LEFT_ENABLED) {
  //   //     // leftMotor.initiateAction(HIGH);
  //   //     initiateAction(HIGH, LEFT);
  //   //   // }
  //   //   // if (RIGHT_ENABLED) {
  //   //     initiateAction(HIGH, RIGHT);
  //   //     // rightMotor.initiateAction(HIGH);
  //   //   // }
  //   //   wireState = 0;
  //   // }

  //   run(LEFT);
  //   run(RIGHT);
  //   // if (LEFT_ENABLED) {
  //   //   // leftMotor.run();
  //   // }
  //   // if (RIGHT_ENABLED) {
  //   //   // rightMotor.run();
  //   // }

  //   // TODO send motorState back to main to control sounds/leds
  //   // if (LEFT_ENABLED) {
  //   //   leftMotorState = leftMotor.getState();
  //   // }
  //   // if (RIGHT_ENABLED) {
  //   //   rightMotorState = rightMotor.getState();
  //   // }

  //   if (printTime >= REPORT_TIME) {
  //     printTime = 0;
  //     if (LEFT_ENABLED) {
  //       report(LEFT);
  //     }
  //     if (RIGHT_ENABLED) {
  //       report(RIGHT);
  //     }
  //   } 

  // }

  // // if (DEBUG && MOTORS_ENABLED && printTime >= REPORT_TIME) {
  // //   printTime = 0;
  // //   if (LEFT_ENABLED) {
  // //     leftMotor.report();
  // //   }
  // //   if (RIGHT_ENABLED) {
  // //     rightMotor.report();
  // //   }
  // // } 

}

