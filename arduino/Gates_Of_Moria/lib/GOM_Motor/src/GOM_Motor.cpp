#include "GOM_Motor.h"

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

volatile int state;     // must survive interrupts
volatile int motorRunningFlag;  // controls the end stop sensor interrupt
volatile int lastMotorRunningFlag;  // controls the end stop sensor interrupt

int runningLEDState = LOW;
int lastRunningLEDState = LOW;

const int MAX_SPEED = 600;
// const int MAX_SPEED = 6000;
const int debounceTime = 200;

// elapsedMillis lastMotorRunTime;
elapsedMillis lastCloseLimitSwitchTime;
elapsedMillis lastToggleTime;
elapsedMillis lastMoveTime;
elapsedMillis lastCloseOpenMoveTime;


GOM_Motor::GOM_Motor(
    bool _debug, 
    AccelStepper _stepper, 
    uint8_t _closeOpenButtonPin, 
    uint8_t _closeLimitSwitchPin, 
    uint8_t _moveButtonPin, 
    uint8_t _directionTogglePin, 
    uint8_t _motorRunningLEDPin, 
    int _closingDirection,
    long _targetOpenPosition
  ) {
    DEBUG = _debug;
    stepper = _stepper;
    closeOpenButtonPin = _closeOpenButtonPin;
    closeLimitSwitchPin = _closeLimitSwitchPin;
    moveButtonPin = _moveButtonPin;
    directionTogglePin = _directionTogglePin;
    motorRunningLEDPin = _motorRunningLEDPin;
    
    closingDirection = _closingDirection;
    currentDirection = _closingDirection;
    openingDirection = -1 * closingDirection;

    targetOpenPosition = _targetOpenPosition;

  // fwdstp = &GOM_Motor::forwardstep;
	// bckwdstp = &GOM_Motor::backwardstep;
	// Adafruit_MotorShield AFMS (IICADDRESS);
	// Adafruit_StepperMotor *AFstepper = AFMS.getStepper(200, 1); //M1 M2
	// AccelStepper stepper(forwardstep, backwardstep);
}
// //go 1 step forward
// void GOM_Motor::forwardstep() {
// 	AFstepper->onestep(FORWARD, DOUBLE);
// }
// //go 1 step backward
// void GOM_Motor::backwardstep() {
// 	AFstepper->onestep(BACKWARD, DOUBLE);
// }


void GOM_Motor::setupMotor() {

  state = INIT;
  // state = RUNSPEED;
  motorRunningFlag = 1;
  lastMotorRunningFlag = motorRunningFlag;

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
      // currentPosition = 0;
      currentDirection = openingDirection;
      state = CLOSED;
    }
  }
  
  pinMode(closeOpenButtonPin, INPUT);
  pinMode(moveButtonPin, INPUT);
  pinMode(directionTogglePin, INPUT_PULLUP);
  // directionToggleState = digitalRead(directionTogglePin);
  // lastDirectionToggleState = directionToggleState;
  // if (directionToggleState == HIGH) {
  //   println("Toggle On");
  //   currentDirection = 1;
  // } else {
  //   println("Toggle Off");
  //   currentDirection = -1;
  // }

  stepper.setMaxSpeed(MAX_SPEED);
  stepper.setSpeed(currentDirection * MAX_SPEED);

  println("");
}

void GOM_Motor::setState() {
  // println("Set State");

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
          // currentPosition = 0;
          currentDirection = openingDirection;
          state = CLOSED;
          // toggleDirection();
        } else {
          state = STOP_BY_CLOSE_LIMIT;
        }
      } else {
        println("Limit Switch Open");
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
          // currentDirection = 1;
        } else {
          println("Toggle Off");
          // currentDirection = -1;
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


  if ( lastCloseOpenMoveTime >= debounceTime) {
    closeOpenButtonState = digitalRead(closeOpenButtonPin);
    if (closeOpenButtonState != lastCloseOpenButtonPinState) {

      if (!initialized) {
        if (closeOpenButtonState == HIGH) {
          println("Starting to close to reset");
          // Start to close the door
          setSpeed(closingDirection * MAX_SPEED);
          state = CLOSING;
        }
      } else {
        // Move
        print("===== LOW/HIGH ");
        print(LOW);
        print(":");
        println(HIGH);
        print("===== closeOpenButtonState ");
        println(closeOpenButtonState);
        if (closeOpenButtonState == HIGH) {
          // open the door
          println("Open the door");
          print("  state now: ");
          println(state);

          if(state == OPEN) {
            // Door at max open position.
            // put into CLOSING state and rely on limit switch to stop
            println("its open...  close now");
            currentSpeed = (closingDirection * MAX_SPEED);
            // stepper.moveTo(0);
            setSpeed(currentSpeed);
            state = CLOSING;

            print(" -> from OPEN. state now:");
            println(state);
          } else if (state == CLOSED) {
            println("its closed...  open");
            currentSpeed = (openingDirection * MAX_SPEED);
            // stepper.move(targetOpenPosition);
            // Must call setSpeed AFTER moveTo
            stepper.moveTo(targetOpenPosition);
            setSpeed(currentSpeed);
            state = OPENING;

            print(" -> from CLOSED. state now:");
            println(state);
          }
        // } else {
        //   print("  else.  state:");
        //   println(state);

        }
      }
      lastCloseOpenButtonPinState = closeOpenButtonState;
      lastCloseOpenMoveTime = 0;
      println("");
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

void GOM_Motor::run() {
  setState();
  // stepper.runSpeed();

  switch(state) {
    case INIT:
      // println("Initializing");
      break;
    case CLOSING:
      // println("CLOSING");
        // if (initialized) {
          runningLEDState = HIGH;
          motorRunningFlag = 1;
          stepper.runSpeed();
        // }
      break;
    case CLOSED:
      motorRunningFlag = 0;
      runningLEDState = LOW;
      currentDirection = openingDirection;
      stepper.setSpeed(0);
      stepper.stop();
      stepper.setCurrentPosition(0);
      stepper.disableOutputs();
      break;
    case OPENING:
      // println("OPENING");
        if (initialized) {
          runningLEDState = HIGH;
          motorRunningFlag = 1;
          // stepper.runSpeed();
          stepper.runSpeedToPosition();
        }
      break;
    case OPEN:
      motorRunningFlag = 0;
      runningLEDState = LOW;
      currentDirection = closingDirection;
      // stepper.setCurrentPosition(0);
      stepper.setSpeed(0);
      stepper.stop();
      stepper.disableOutputs();
      break;
    case STOP:
      motorRunningFlag = 0;
      runningLEDState = LOW;
      // if (motorRunningFlag != lastMotorRunningFlag) {
// stepper.runToPosition();
        setSpeed(0);
        stepper.stop();
        stepper.disableOutputs();
      // }
      break;
    case STOP_NOW:
      motorRunningFlag = 0;
      runningLEDState = LOW;
      // if (motorRunningFlag != lastMotorRunningFlag) {
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
      // }
      break;
    case RUNSPEED:
      // motorRunningFlag = 1;
      // if (motorRunningFlag != lastMotorRunningFlag) {
        runningLEDState = HIGH;
        motorRunningFlag = 1;
        stepper.runSpeed();
      // }
      break;
  //   case RUN:
  //     runningLEDState = HIGH;
  //     motorRunningFlag = 1;
  //   //   setSpeed(currentSpeed);
  //   //   stepper.run();
  //   //   lastMotorRunTime = 0;
  //     break;
  //   case STOP_BY_CLOSE_LIMIT:
  //     if (motorRunningFlag == 1) {
  //       println("STOP_BY_CLOSE_LIMIT");
  //     }
  //     runningLEDState = LOW;
  //     motorRunningFlag = 0;
  //     stepper.stop();
  //     break;
  }

  // Run states
  lastMotorRunningFlag = motorRunningFlag;

  toggleRunningLEDIfNeeded();


  // if (lastMotorRunTime >= 4000) {
  //   println("Stopping motor");
  //   lastMotorRunTime = 0;
  //   stepper.disableOutputs();
  // } 
}

void GOM_Motor::setSpeed(int speed) {
  // println(stepper.speed());
  stepper.setSpeed(speed);
  // print("   Set speed ");
  // println(speed);
  // println(stepper.speed());
}

// Turns on/off running LED if running state changed
void GOM_Motor::toggleRunningLEDIfNeeded() {
  if(runningLEDState != lastRunningLEDState) {
    digitalWrite(motorRunningLEDPin, runningLEDState);
    lastRunningLEDState = runningLEDState;
  }
}

// toggleDirection
void GOM_Motor::toggleDirection() {
  currentDirection = -1 * currentDirection;
}


// Called from main app when stop button pressed
void GOM_Motor::stopEverything(String name) {
  print(name);
  println(" stopEverything called");
  state = STOP;
}

void GOM_Motor::print(String val) {
  if (DEBUG) {
    Serial.print(val);
  }
}
void GOM_Motor::println(String val) {
  if (DEBUG) {
    Serial.println(val);
  }
}
void GOM_Motor::print(long val) {
  if (DEBUG) {
    Serial.print(val);
  }
}
void GOM_Motor::println(long val) {
  if (DEBUG) {
    Serial.println(val);
  }
}

void GOM_Motor::report(String name) {
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
    // print("  currentPosition: ");
    // print(currentPosition);
    print(" trgt: ");
    println(targetOpenPosition);
    println("-----------------------------------------------------");
}
