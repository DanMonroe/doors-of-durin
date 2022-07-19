// gatesofmoria_motor.cpp
#include "GatesOfMoria_Motor.h"

#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

const uint16_t MAX_SPEED = 6000;
const uint8_t DEBOUNCE_INDEX_TOGGLE = 0;
const uint8_t DEBOUNCE_INDEX_MOVE_BUTTON = 1;
const uint8_t DEBOUNCE_INDEX_CLOSE_LIMIT_SWITCH = 2;

// State definitions
#define RUNSPEED 01
#define RUN 02
#define RUN_HOME 03
#define STOP 04
#define STOP_NOW 05

// State variable
volatile int state; // must survive interrupts
// volatile int enabFlag;  // controls the end stop sensor interrupt

int runningLEDState = LOW;
int lastRunningLEDState = LOW;
int lastClosedLimitSwitchState = LOW;

int debounceTime = 50;


const int sensorPin = 9;  // used as an end stop sensor output - driven by a timer
// hook to pin D2 - or can drive D2 directly (tie low, then disconnect to cause interrupt)

// elapsedMillis printTime;
elapsedMillis lastMotorRunTime;
elapsedMillis lastToggleTime;
elapsedMillis lastMoveTime;
elapsedMillis lastCloseLimitTime;


GatesOfMoria_Motor::GatesOfMoria_Motor(bool _debug, AccelStepper _stepper, uint8_t _closedLimitSwitchPin, uint8_t _moveButtonPin, uint8_t _directionTogglePin, uint8_t _motorRunningLEDPin) {
  DEBUG = _debug;
  stepper = _stepper;
  closedLimitSwitchPin = _closedLimitSwitchPin;
  moveButtonPin = _moveButtonPin;
  directionTogglePin = _directionTogglePin;
  motorRunningLEDPin = _motorRunningLEDPin;

}

void GatesOfMoria_Motor::setupMotor() {
  // attachInterrupt(digitalPinToInterrupt(closedLimitSwitchPin), rising, RISING);
  // attachInterrupt(0, rising, RISING);  // Interrupt 0 is pin D2

  print("closedLimitSwitchPin ");
  println(closedLimitSwitchPin);

  pinMode(motorRunningLEDPin, OUTPUT);
  digitalWrite(motorRunningLEDPin, runningLEDState);  // Start off

  // enabFlag = 1; // enable the sensor interrupt
  pinMode (closedLimitSwitchPin, INPUT);
  // pinMode (closedLimitSwitchPin, INPUT_PULLUP);
  // pinMode (closedLimitSwitchPin, OUTPUT);
  state = RUN;   // initial state is run, just run
  attachInterrupt(digitalPinToInterrupt(closedLimitSwitchPin), rising, RISING);

  stepper.setMaxSpeed(MAX_SPEED);
  stepper.setSpeed(MAX_SPEED);
  // stepper.setAcceleration(1000);
  
  // initialize the pushbutton pin as an input:
  pinMode(moveButtonPin, INPUT);
  // pinMode(moveButtonPin, INPUT_PULLUP);
  pinMode(directionTogglePin, INPUT_PULLUP);

  directionToggleState = digitalRead(directionTogglePin);
  if (directionToggleState == HIGH) {
    println("Toggle On");
    current_direction = FORWARD;
  } else {
    println("Toggle Off");
    current_direction = BACKWARD;
  }



      print("limit state ");
    print(closedLimitSwitchState);
    print(" ");
    println(lastClosedLimitSwitchState);
}

void GatesOfMoria_Motor::run() {
  setState();

  switch(state) {
    case RUNSPEED:
      runningLEDState = HIGH;
    //   stepper.runSpeed();
      break;
    case RUN:
      runningLEDState = HIGH;
    //   setSpeed(currentSpeed);
    //   stepper.run();
    //   lastMotorRunTime = 0;
      break;
    case STOP:
      runningLEDState = LOW;
      stepper.stop();
      break;
    case STOP_NOW:
      runningLEDState = LOW;
      // digitalWrite(sensorPin,LOW);    // removes interrupt signal
      // stepper.setAcceleration(200.0);  // this makes motor stop much quicker!
      stepper.stop();
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

void GatesOfMoria_Motor::setSpeed(int speed) {
  println(stepper.speed());
  stepper.setSpeed(speed);
  print("   Set speed");
  println(speed);
  println(stepper.speed());
}

// Turns on/off running LED if running state changed
void GatesOfMoria_Motor::toggleRunningLEDIfNeeded() {
  if(runningLEDState != lastRunningLEDState) {
    digitalWrite(motorRunningLEDPin, runningLEDState);
    lastRunningLEDState = runningLEDState;
  }
}

void GatesOfMoria_Motor::setState() {
// println("Set State");
  // Close limit switch
  // if ( (millis() - previousMillis[DEBOUNCE_INDEX_CLOSE_LIMIT_SWITCH]) >= debounceTime) {

  if ( lastCloseLimitTime >= debounceTime) {
    closedLimitSwitchState = digitalRead(closedLimitSwitchPin);
    if (closedLimitSwitchState != lastClosedLimitSwitchState) {
      if (closedLimitSwitchState == LOW) {
        state = STOP;
      }
      lastClosedLimitSwitchState = closedLimitSwitchState;
    }
    lastCloseLimitTime = 0;
  }

  // direction toggle
  if ( lastToggleTime >= debounceTime) {
    directionToggleState = digitalRead(directionTogglePin);
    if (directionToggleState != lastDirectionToggleState) {
      if (directionToggleState == HIGH) {
        println("Toggle On");
        current_direction = FORWARD;
      } else {
        println("Toggle Off");
        current_direction = BACKWARD;
      }
      print("current direction ");
      println(current_direction);
    }
    lastToggleTime = 0;
    lastDirectionToggleState = directionToggleState;
  }


  if ( lastMoveTime >= debounceTime) {
    moveButtonState = digitalRead(moveButtonPin);
    if (moveButtonState != lastMoveButtonState) {
      // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
      if (moveButtonState == HIGH) {
        println("Motor On");
        currentSpeed = (current_direction == FORWARD ? MAX_SPEED : -MAX_SPEED);
        setSpeed(currentSpeed);
        state = RUN;
      } else {
        state = STOP;
        println("Motor Off");
      }

      lastMoveButtonState = moveButtonState;
      lastMoveTime = 0;
      // previousMillis[DEBOUNCE_INDEX_MOVE_BUTTON] = millis();

      println("");
    }
  }
}

// Called from main app when stop button pressed
void GatesOfMoria_Motor::stopEverything(String name) {
  print(name);
  println(" stopEverything called");
  state = STOP;
}

void GatesOfMoria_Motor::print(String val) {
  if (DEBUG) {
    Serial.print(val);
  }
}
void GatesOfMoria_Motor::println(String val) {
  if (DEBUG) {
    Serial.println(val);
  }
}
void GatesOfMoria_Motor::print(long val) {
  if (DEBUG) {
    Serial.print(val);
  }
}
void GatesOfMoria_Motor::println(long val) {
  if (DEBUG) {
    Serial.println(val);
  }
}
// void GatesOfMoria_Motor::print(float val) {
//   if (DEBUG) {
//     Serial.print(val);
//   }
// }
// void GatesOfMoria_Motor::println(float val) {
//   if (DEBUG) {
//     Serial.println(val);
//   }
// }
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++ Interrupt service routine +++++++++++++++++++++++++++++
//  If enable flag is true, enter state STOP_NOW
void GatesOfMoria_Motor::rising() {
  // if (enabFlag == 1) {
  //   state = STOP_NOW;
  //   enabFlag = 0;
  // }
}

void GatesOfMoria_Motor::report(String name) {
    float mSpeed = stepper.speed();
    print(name);
    print(" state: ");
    print(state);
    print(" speed: ");
    print(mSpeed);
    print("  pos: ");
    println(stepper.currentPosition());

}
