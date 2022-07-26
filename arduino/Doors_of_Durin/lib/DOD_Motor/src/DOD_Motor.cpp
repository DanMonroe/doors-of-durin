#include "DOD_Motor.h"

#include <Arduino.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>


// State definitions
#define INIT 1
#define CLOSING 2
#define CLOSED 3
#define RUNSPEED 4
#define RUN 5
#define RUN_HOME 6
#define STOP 7
#define STOP_NOW 8
#define STOP_BY_CLOSE_LIMIT 9
#define OPENING 10
#define OPEN 11

volatile int state;

int runningLEDState = LOW;
int lastRunningLEDState = LOW;

const int MAX_SPEED = 600;
// const int MAX_SPEED = 6000;
const int debounceTime = 200;

elapsedMillis lastCloseLimitSwitchTime;
elapsedMillis lastToggleTime;
elapsedMillis lastMoveTime;


DOD_Motor::DOD_Motor(
    bool _debug, 
    AccelStepper _stepper, 
    uint8_t _closeLimitSwitchPin, 
    uint8_t _moveButtonPin, 
    uint8_t _directionTogglePin, 
    uint8_t _motorRunningLEDPin, 
    int _closingDirection,
    long _targetOpenPosition
  ) {
    DEBUG = _debug;
    stepper = _stepper;
    closeLimitSwitchPin = _closeLimitSwitchPin;
    moveButtonPin = _moveButtonPin;
    directionTogglePin = _directionTogglePin;
    motorRunningLEDPin = _motorRunningLEDPin;
    
    closingDirection = _closingDirection;
    currentDirection = _closingDirection;
    openingDirection = -1 * closingDirection;

    targetOpenPosition = _targetOpenPosition;

  // fwdstp = &DOD_Motor::forwardstep;
	// bckwdstp = &DOD_Motor::backwardstep;
	// Adafruit_MotorShield AFMS (IICADDRESS);
	// Adafruit_StepperMotor *AFstepper = AFMS.getStepper(200, 1); //M1 M2
	// AccelStepper stepper(forwardstep, backwardstep);
}
// //go 1 step forward
// void DOD_Motor::forwardstep() {
// 	AFstepper->onestep(FORWARD, DOUBLE);
// }
// //go 1 step backward
// void DOD_Motor::backwardstep() {
// 	AFstepper->onestep(BACKWARD, DOUBLE);
// }


void DOD_Motor::setupMotor() {

  state = INIT;

  pinMode(motorRunningLEDPin, OUTPUT);
  digitalWrite(motorRunningLEDPin, runningLEDState);  // Start off

  print("closedLimitSwitchPin ");
  println(closeLimitSwitchPin);
  pinMode(closeLimitSwitchPin, INPUT_PULLUP);
  closeLimitSwitchState = digitalRead(closeLimitSwitchPin);
  lastCloseLimitSwitchState = closeLimitSwitchState;
  if (closeLimitSwitchState == LOW) {
    state = STOP_BY_CLOSE_LIMIT;
    if (!initialized) {
      initialized = true;
      currentDirection = openingDirection;
      state = CLOSED;
    }
  }
  
  pinMode(moveButtonPin, INPUT);
  pinMode(directionTogglePin, INPUT_PULLUP);

  stepper.setMaxSpeed(MAX_SPEED);
  stepper.setAcceleration(2);
  stepper.setSpeed(currentDirection * MAX_SPEED);

  println("");
}

void DOD_Motor::setState() {
  // Close Limit Switch
  if ( lastCloseLimitSwitchTime >= debounceTime) {
    closeLimitSwitchState = digitalRead(closeLimitSwitchPin);
    if (closeLimitSwitchState != lastCloseLimitSwitchState) {
      if (closeLimitSwitchState == LOW) {
        println("Limit Switch Closed");

        // If starting while the close limit switch is closed, set ready to open.
        if (!initialized || state == CLOSING) {
          println("Initialized");
          initialized = true;
          currentDirection = openingDirection;
          state = CLOSED;
        } else {
          state = STOP_BY_CLOSE_LIMIT;
        }
      }
      lastCloseLimitSwitchState = closeLimitSwitchState;
      lastCloseLimitSwitchTime = 0;
    }
  }

  // direction toggle only if the door is not closed.
  if (state != CLOSED) {
    if ( lastToggleTime >= debounceTime) {
      directionToggleState = digitalRead(directionTogglePin);
      if (directionToggleState != lastDirectionToggleState) {
        if (directionToggleState == HIGH) {
          println("Toggle On");
        } else {
          println("Toggle Off");
        }
        toggleDirection();
        print("current direction ");
        println(currentDirection);
        currentSpeed = (currentDirection * MAX_SPEED);
        setSpeed(currentSpeed);
        lastDirectionToggleState = directionToggleState;
        lastToggleTime = 0;
      }
    }
  }

  if ( lastMoveTime >= debounceTime) {
    moveButtonState = digitalRead(moveButtonPin);
    if (moveButtonState != lastMoveButtonState) {
      // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
      // Move
      if (moveButtonState == HIGH) {
        println("Motor On");
        currentSpeed = (currentDirection * MAX_SPEED);
        setSpeed(currentSpeed);
        state = RUNSPEED;
      } else {
        state = STOP;
        println("Motor Off");
      }
      lastMoveButtonState = moveButtonState;
      lastMoveTime = 0;
      println("");
    }
  }

  if (state == OPENING && stepper.distanceToGo() == 0) {
    state = OPEN;
  }
}

void DOD_Motor::run() {
  setState();

  // INIT, RUNSPEED fall through to CLOSING
  // CLOSED, OPEN, STOP fall through to STOP_NOW
  switch(state) {
    case INIT:
      break;
    case RUNSPEED:
    case CLOSING:
      runningLEDState = HIGH;
      stepper.runSpeed();
      break;
    case OPENING:
      if (initialized) {
        runningLEDState = HIGH;
        stepper.runSpeedToPosition();
      }
      break;
    case CLOSED:
      currentDirection = openingDirection;
      stepper.setCurrentPosition(0);
    case OPEN:
      currentDirection = closingDirection;
    case STOP:
    case STOP_NOW:
      runningLEDState = LOW;
        // digitalWrite(sensorPin,LOW);    // removes interrupt signal
        // stepper.setAcceleration(200.0);  // this makes motor stop much quicker!
// stepper.runToPosition();
        setSpeed(0);
        stepper.stop();
        stepper.disableOutputs();
        // stepper.runToPosition();  // brings to a stop!
        // stepper.moveTo(0);  // now return to home position
        // stepper.setAcceleration(50.0);  // slow motor acceleration back down
        //myStepper.move(-myStepper.currentPosition()); // This should work also
        // state = RUN_HOME;
      break;
  }

  toggleRunningLEDIfNeeded();


  // if (lastMotorRunTime >= 4000) {
  //   println("Stopping motor");
  //   lastMotorRunTime = 0;
  //   stepper.disableOutputs();
  // } 
}

void DOD_Motor::setSpeed(int speed) {
  stepper.setSpeed(speed);
}

// Turns on/off running LED if running state changed
void DOD_Motor::toggleRunningLEDIfNeeded() {
  if(runningLEDState != lastRunningLEDState) {
    digitalWrite(motorRunningLEDPin, runningLEDState);
    lastRunningLEDState = runningLEDState;
  }
}

// toggleDirection
void DOD_Motor::toggleDirection() {
  currentDirection = -1 * currentDirection;
}

// Called from the loop of main.cpp
void DOD_Motor::initiateAction(int actionButtonState) {   // HIGH or LOW
  if (actionButtonState == HIGH) {
    print("INITIATE ACTION BUTTON PRESSED ");
    println(actionButtonState);

    if (!initialized) {
      println("Starting to close to reset");
      // Start to close the door
      setSpeed(closingDirection * MAX_SPEED);
      state = CLOSING;
    } else {
      if(state == OPEN) {
        // Door at max open position.
        // put into CLOSING state and rely on limit switch to stop
        currentSpeed = (closingDirection * MAX_SPEED);
        setSpeed(currentSpeed);
        state = CLOSING;
      } else if (state == CLOSED) {
        currentSpeed = (openingDirection * MAX_SPEED);
        // Must call setSpeed AFTER moveTo
        stepper.moveTo(targetOpenPosition);
        setSpeed(currentSpeed);
        state = OPENING;
      }
    }
  }
}

// Called from main app when stop button pressed
void DOD_Motor::stopEverything(String name) {
  print(name);
  println(" stopEverything called");
  state = STOP;
}

void DOD_Motor::print(String val) {
  if (DEBUG) {
    Serial.print(val);
  }
}
void DOD_Motor::println(String val) {
  if (DEBUG) {
    Serial.println(val);
  }
}
void DOD_Motor::print(long val) {
  if (DEBUG) {
    Serial.print(val);
  }
}
void DOD_Motor::println(long val) {
  if (DEBUG) {
    Serial.println(val);
  }
}

void DOD_Motor::report(String name) {
    float mSpeed = stepper.speed();
    println(name);
    print(" state: ");
    switch(state) {
      case INIT:
        println("INIT");
        break;
      case CLOSING:
        println("CLOSING");
        break;
      case CLOSED:
        println("CLOSED");
        break;
      case RUNSPEED:
        println("RUNSPEED");
        break;
      case RUN:
        println("RUN");
        break;
      case RUN_HOME:
        println("RUN_HOME");
        break;
      case STOP:
        println("STOP");
        break;
      case STOP_NOW:
        println("STOP_NOW");
        break;
      case STOP_BY_CLOSE_LIMIT:
        println("STOP_BY_CLOSE_LIMIT");
        break;
      case OPENING:
        println("OPENING");
        break;
      case OPEN:
        println("OPEN");
        break;
      default:
        println("UNKNOWN!!!!!");
        break;
    }
    print(" spd: ");
    print(mSpeed);
    print(" pos: ");
    print(stepper.currentPosition());
    print(" trgt: ");
    println(targetOpenPosition);
    println("-----------------------------------------------------");
}
