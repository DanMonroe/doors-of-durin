#include <Arduino.h>

#include "pins.h"
// #include <DOD_Sound.h>
// #include <DOD_PixelStrips.h>

// #include <AccelStepper.h>
#include <Wire.h>
// #include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

#include <SPI.h>
// #include <Adafruit_VS1053.h>
#include <SD.h>

// #include "pins.h"
#include "motorStates.h"
// #include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>


volatile int stopButtonState = 0;           // current state of the stop button (on interrupt)
// uint8_t initiateActionButtonState = 0;      // current state of the action button
// uint8_t lastInitiateActionButtonState = 0;

// const int initiateActionButtonPin = 10;  // using DOD_PIN_initiateActionButtonPin now
int initiateActionButtonState = 0;
int lastInitiateActionButtonState = -1;
elapsedMillis lastInitiateActionTime;

// int motorState = 0;

const bool MOTORS_ENABLED = true;
const bool SOUND_ENABLED = false;
// const bool LEDS_ENABLED = true;

const bool DEBUG = true;
const int debounceTime = 150;


// From motor_main ********************************** start
// const bool MOTORS_ENABLED = true;
const bool LEFT_ENABLED = true;
const bool RIGHT_ENABLED = false;

unsigned int REPORT_TIME = 2500;

// int wireState = 0;
// int lastWireState = -1;

// elapsedMillis printTime;

// define Slave I2C address
// #define SLAVE_ADDR 9
// #define SIGNAL_INITIATE_BUTTON_HIGH 200

#define LEFT 0
#define RIGHT 1

const int MAX_SPEED = 100;  // ~= 19 seconds per revolution
// const int debounceTime = 200;

int motorState[2] = {MOTOR_STATE_NOTHING, MOTOR_STATE_NOTHING};
int lastPrintMotorState[2] = {MOTOR_STATE_INIT, MOTOR_STATE_INIT};
int motorAcceleration[2] = {200, 200};
int closeLimitSwitchPin[2] = {12, 11}; 
int closeLimitSwitchState[2] = {0, 0};
int lastCloseLimitSwitchState[2] = {-1, -1};
elapsedMillis lastCloseLimitSwitchTime[2];
elapsedMillis rampUpTime[2];
int closingDirection[2] = {-1, 1};
int openingDirection[2] = {1, -1};

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
void printName(int motorIndex) {
  if (motorIndex == LEFT) {
    print("LEFT: ", motorIndex);
  } else {
    print("RIGHT: ", motorIndex);
  }
}
void printMotorState(int motorIndex) {
  if (DEBUG) {
    // if (lastPrintMotorState[motorIndex] != motorState[motorIndex]) {
      // lastPrintMotorState[motorIndex] = motorState[motorIndex];

      printName(motorIndex);
      print("State: ", motorIndex);
      switch (motorState[motorIndex]) {
        case MOTOR_STATE_NOTHING:
            print("MOTOR_STATE_NOTHING", motorIndex);
          break;
        case MOTOR_STATE_RUN_CLOSING:
          // Closing start
          print("MOTOR_STATE_RUN_CLOSING", motorIndex);
          break;
        case MOTOR_STATE_RUNSPEED_CLOSING:
          print("MOTOR_STATE_RUNSPEED_CLOSING", motorIndex);
          // Closing 
          break;
        case MOTOR_STATE_STOP:
          print("MOTOR_STATE_STOP", motorIndex);
          // Stopped
          // dont call run
          break;
        case MOTOR_STATE_RUN_OPENING:
          print("MOTOR_STATE_RUN_OPENING", motorIndex);
          // Opening start
          break;
        case MOTOR_STATE_RUNSPEED_OPENING:
          print("MOTOR_STATE_RUNSPEED_OPENING", motorIndex);
          // Opening
          break;
        
        default:
          print("MOTOR_STATE_NOTHING", motorIndex);
          break;
      }
      print(" (", motorIndex);
      print(motorState[motorIndex], motorIndex);
      println(")", motorIndex);
    }
  // }
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

  motorState[motorIndex]++;   // WHY are incrementing here?  9 Feb 24

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
        if (DEBUG) {
          Serial.println("");
          printName(motorIndex);
          Serial.print("Opening");
          Serial.println("");
        }
      }
      break;

    default:
      break;
  }
}

void checkLimitSwitchClosed(int motorIndex) {
  closeLimitSwitchState[motorIndex] = digitalRead(closeLimitSwitchPin[motorIndex]);
// print("checkLimitSwitchClosed 0: ", motorIndex);
// print("closeLimitSwitchState: ", motorIndex);
// print(closeLimitSwitchState[motorIndex], motorIndex);
// print("  /  lastCloseLimitSwitchState: ", motorIndex);
// println(lastCloseLimitSwitchState[motorIndex], motorIndex);
  if (closeLimitSwitchState[motorIndex] != lastCloseLimitSwitchState[motorIndex]) {
    if (closeLimitSwitchState[motorIndex] == LOW) {

      print("checkLimitSwitchClosed 1: ", motorIndex);
      printMotorState(motorIndex);

      // if closing or closed already
      if (motorState[motorIndex] < MOTOR_STATE_STOP) {
        motorState[motorIndex] = MOTOR_STATE_STOP;
        print("checkLimitSwitchClosed 2: ", motorIndex);
        printMotorState(motorIndex);
      }
    }
    lastCloseLimitSwitchState[motorIndex] = closeLimitSwitchState[motorIndex];
  }
}

void motor_setup() {
  AFMS.begin();

  if (DEBUG) {
    Serial.println("motor setup 1");
  }
  if (motorEnabled(LEFT)) {
    motorState[LEFT] = MOTOR_STATE_NOTHING;
    pinMode(closeLimitSwitchPin[LEFT], INPUT_PULLUP);

    if (DEBUG) {
      printName(LEFT);
      Serial.print("motor setup");
    }

    checkLimitSwitchClosed(LEFT);
  }
  if (motorEnabled(RIGHT)) {
    motorState[RIGHT] = MOTOR_STATE_NOTHING;
    pinMode(closeLimitSwitchPin[RIGHT], INPUT_PULLUP);

    if (DEBUG) {
      printName(RIGHT);
      Serial.print("motor setup");
    }
    checkLimitSwitchClosed(RIGHT);
  }

  delay(1500);  // avoids startup pause
}

void motor_loop() {
  for (int motorIndex=0; motorIndex<MOTOR_STATE_RUNSPEED_CLOSING; motorIndex++) {
    if (motorEnabled(motorIndex)) {
// print("lastCloseLimitSwitchTime: ", motorIndex);
// println(lastCloseLimitSwitchTime[motorIndex], motorIndex);
      if ( lastCloseLimitSwitchTime[motorIndex] >= debounceTime) {
        checkLimitSwitchClosed(motorIndex);
        lastCloseLimitSwitchTime[motorIndex] = 0;
      }
if (true) {
      float mSpeed;
      // rampUp
      if (rampUpTime[motorIndex] >= 500) {
        rampUpTime[motorIndex] = 0;
        mSpeed = rightStepper.speed();
        if (DEBUG) {
          Serial.print(mSpeed);
          Serial.print("  ");
          Serial.print(rightStepper.currentPosition());
          Serial.print("  ");
          Serial.print(motorState[motorIndex]);
          Serial.print("  ");
          Serial.println(rightStepper.distanceToGo());
        }
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
      if (DEBUG) {
        Serial.print("Loop: ");
        printMotorState(motorIndex);
      }
} // if false      
    }
  } // for motorIndex

}
// From motor_main ********************************** end

// unsigned int REPORT_TIME = 2500;

// int wireState = 0;
// int lastWireState = -1;

elapsedMillis printTime;



// DOD_PixelStrips *strips = new DOD_PixelStrips();

// DOD_Sound *sound = new DOD_Sound(DEBUG);

// Interrupt when the STOP button is pressed
// Set state on all motors to STOP
void stopEverything() {

  if (MOTORS_ENABLED) {
    // leftMotor.stopEverything("Motor 1");
    // rightMotor.stopEverything("Motor 2");
    // rightStepper->release();

    if (motorEnabled(LEFT)) {
      println("", LEFT);
      println("stopEverything LEFT: ", LEFT);
      println("", LEFT);
      motorState[LEFT] = MOTOR_STATE_NOTHING; // or MOTOR_STATE_STOP 
      // motorState[LEFT] = MOTOR_STATE_STOP;
    }
    if (motorEnabled(RIGHT)) {
      println("", RIGHT);
      println("stopEverything RIGHT: ", RIGHT);
      println("", RIGHT);
      motorState[RIGHT] = MOTOR_STATE_NOTHING;  // or MOTOR_STATE_STOP ?
      // motorState[RIGHT] = MOTOR_STATE_STOP;
    }
    // Wire.beginTransmission(SLAVE_ADDR);
    // Wire.write(SIGNAL_STOP_EVERYTHING);
    // Wire.endTransmission();

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
        initiateAction(LEFT);
        initiateAction(RIGHT);
        
        // Wire.beginTransmission(SLAVE_ADDR);
        // if (initiateActionButtonState == HIGH) {
        //   if (DEBUG) {
        //     Serial.println("checkInitiateActionButton writing 200");
        //   }
        //   Wire.write(SIGNAL_INITIATE_BUTTON_HIGH);
        // }
        // Wire.endTransmission();
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

  delay( 2000 ); // power-up safety delay

  // if (MOTORS_ENABLED) {
  //   // Initialize I2C communications as Master
  //   // Wire.begin();
  //   motor_setup();
  // }

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

  initiateActionButtonState = digitalRead(DOD_PIN_initiateActionButtonPin);
  lastInitiateActionButtonState = initiateActionButtonState;
  lastInitiateActionTime = 0;

  if (MOTORS_ENABLED) {
    // Initialize I2C communications as Master
    // Wire.begin();
    motor_setup();
  }

  // if (SOUND_ENABLED) {
  //   sound->setup();
  // }

}

void loop() {

  checkInitiateActionButton();

  motor_loop();  // UNCOMMENT THIS LINE

  // if (SOUND_ENABLED) {
  //   sound->run(motorState);
  // }

}