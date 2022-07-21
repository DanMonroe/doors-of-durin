#include "GOM_Motor.h"

#include <Arduino.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>


// State definitions
#define RUNSPEED 01
#define RUN 02
#define RUN_HOME 03
#define STOP 04
#define STOP_NOW 05
#define STOP_BY_CLOSE_LIMIT 06

volatile int state;     // must survive interrupts
volatile int motorRunningFlag;  // controls the end stop sensor interrupt
volatile int lastMotorRunningFlag;  // controls the end stop sensor interrupt

int runningLEDState = LOW;
int lastRunningLEDState = LOW;

const int MAX_SPEED = 600;
// const int MAX_SPEED = 6000;
const int debounceTime = 50;

// elapsedMillis lastMotorRunTime;
elapsedMillis lastCloseLimitSwitchTime;
elapsedMillis lastToggleTime;
elapsedMillis lastMoveTime;


GOM_Motor::GOM_Motor(bool _debug, AccelStepper _stepper, uint8_t _closeLimitSwitchPin, uint8_t _moveButtonPin, uint8_t _directionTogglePin, uint8_t _motorRunningLEDPin) {
  DEBUG = _debug;
  stepper = _stepper;
  closeLimitSwitchPin = _closeLimitSwitchPin;
  moveButtonPin = _moveButtonPin;
  directionTogglePin = _directionTogglePin;
  motorRunningLEDPin = _motorRunningLEDPin;

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

  state = RUNSPEED;
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
    state = STOP;
    // runningLEDState = LOW;
  }
  
  pinMode(moveButtonPin, INPUT);
  pinMode(directionTogglePin, INPUT_PULLUP);
  directionToggleState = digitalRead(directionTogglePin);
  lastDirectionToggleState = directionToggleState;
  if (directionToggleState == HIGH) {
    println("Toggle On");
    current_direction = 1;
  } else {
    println("Toggle Off");
    current_direction = -1;
  }

  stepper.setMaxSpeed(MAX_SPEED);
  stepper.setSpeed(current_direction * MAX_SPEED);

  println("");
}

void GOM_Motor::run() {
  setState();
  // stepper.runSpeed();

  switch(state) {
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
    case STOP:
      motorRunningFlag = 0;
      runningLEDState = LOW;
      // if (motorRunningFlag != lastMotorRunningFlag) {
        stepper.runToPosition();
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
        stepper.runToPosition();
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
  }
  lastMotorRunningFlag = motorRunningFlag;

  toggleRunningLEDIfNeeded();


  // if (lastMotorRunTime >= 4000) {
  //   println("Stopping motor");
  //   lastMotorRunTime = 0;
  //   stepper.disableOutputs();
  // } 
}

void GOM_Motor::setSpeed(int speed) {
  println(stepper.speed());
  stepper.setSpeed(speed);
  // stepper.setSpeed(speed);
  print("   Set speed ");
  println(speed);
  println(stepper.speed());
}

// Turns on/off running LED if running state changed
void GOM_Motor::toggleRunningLEDIfNeeded() {
  if(runningLEDState != lastRunningLEDState) {
    digitalWrite(motorRunningLEDPin, runningLEDState);
    lastRunningLEDState = runningLEDState;
  }
}

void GOM_Motor::setState() {
  // println("Set State");

  // Close Limit Switch
  if ( lastCloseLimitSwitchTime >= debounceTime) {
    closeLimitSwitchState = digitalRead(closeLimitSwitchPin);
    if (closeLimitSwitchState != lastCloseLimitSwitchState) {
      if (closeLimitSwitchState == LOW) {
        println("Limit Switch Closed");
        state = STOP_BY_CLOSE_LIMIT;
      } else {
        println("Limit Switch Open");
      }
      lastCloseLimitSwitchState = closeLimitSwitchState;
      lastCloseLimitSwitchTime = 0;
    }
  }

  // direction toggle
  if ( lastToggleTime >= debounceTime) {
    directionToggleState = digitalRead(directionTogglePin);
    if (directionToggleState != lastDirectionToggleState) {
      if (directionToggleState == HIGH) {
        println("Toggle On");
        current_direction = 1;
      } else {
        println("Toggle Off");
        current_direction = -1;
      }
      print("current direction ");
      println(current_direction);
      currentSpeed = (current_direction * MAX_SPEED);
      setSpeed(currentSpeed);
      lastDirectionToggleState = directionToggleState;
      lastToggleTime = 0;
    }
  }


  if ( lastMoveTime >= debounceTime) {
    moveButtonState = digitalRead(moveButtonPin);
    if (moveButtonState != lastMoveButtonState) {
      // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
      if (moveButtonState == HIGH) {
        println("Motor On");
        currentSpeed = (current_direction * MAX_SPEED);
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
    print(name);
    print(" state: ");
    print(state);
    print(" speed: ");
    print(mSpeed);
    print("  pos: ");
    println(stepper.currentPosition());
}
