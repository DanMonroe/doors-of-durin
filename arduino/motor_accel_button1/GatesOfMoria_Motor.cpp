// gatesofmoria_motor.cpp
#include "GatesOfMoria_Motor.h"

#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

const uint16_t MAX_SPEED = 6000;
const uint8_t DEBOUNCE_INDEX_TOGGLE = 0;
const uint8_t DEBOUNCE_INDEX_MOVE_BUTTON = 1;

// State definitions
#define RUNSPEED 01
#define RUN 02
#define STOP 03

// State variable
int state;

// elapsedMillis printTime;
elapsedMillis lastMotorRunTime;

GatesOfMoria_Motor::GatesOfMoria_Motor(AccelStepper _stepper, uint8_t _moveButtonPin, uint8_t _directionTogglePin) {
  stepper = _stepper;
  moveButtonPin = _moveButtonPin;
  directionTogglePin = _directionTogglePin;
}

void GatesOfMoria_Motor::setupMotor() {
  stepper.setMaxSpeed(MAX_SPEED);
  stepper.setSpeed(MAX_SPEED);
  // stepper.setAcceleration(1000);
  
  // initialize the pushbutton pin as an input:
  pinMode(moveButtonPin, INPUT);
  pinMode(directionTogglePin, INPUT);

  directionToggleState = digitalRead(directionTogglePin);
  if (directionToggleState == HIGH) {
    Serial.println("Toggle On");
    current_direction = FORWARD;
  } else {
    Serial.println("Toggle Off");
    current_direction = BACKWARD;
  }
  // Serial.print("Initial current direction ");
  // Serial.println(current_direction);

  previousMillis[DEBOUNCE_INDEX_TOGGLE] = millis();
  previousMillis[DEBOUNCE_INDEX_MOVE_BUTTON] = millis();
}

void GatesOfMoria_Motor::run() {
  setState();

  switch(state) {
    case RUNSPEED:
      break;
    case RUN:
      setSpeed(currentSpeed);
      stepper.run();
      lastMotorRunTime = 0;
      break;
    case STOP:
      stepper.stop();
      break;
  }

  if (lastMotorRunTime >= 4000) {
    Serial.println("Stopping motor");
    lastMotorRunTime = 0;
    stepper.disableOutputs();
  } 
}

void GatesOfMoria_Motor::setSpeed(int speed) {
  Serial.println(stepper.speed());
  Serial.print("   Set speed");
  Serial.println(speed);
  stepper.setSpeed(speed);
  Serial.println(stepper.speed());
}

void GatesOfMoria_Motor::setState() {
  if ( (millis() - previousMillis[DEBOUNCE_INDEX_TOGGLE]) >= 50) {
    directionToggleState = digitalRead(directionTogglePin);
    if (directionToggleState != lastDirectionToggleState) {
      if (directionToggleState == HIGH) {
        Serial.println("Toggle On");
        current_direction = FORWARD;
      } else {
        Serial.println("Toggle Off");
        current_direction = BACKWARD;
      }
      Serial.print("current direction ");
      Serial.println(current_direction);
    }
    previousMillis[DEBOUNCE_INDEX_TOGGLE] = millis();
    lastDirectionToggleState = directionToggleState;
  }


  if ( (millis() - previousMillis[DEBOUNCE_INDEX_MOVE_BUTTON]) >= 50) {
    moveButtonState = digitalRead(moveButtonPin);
    if (moveButtonState != lastMoveButtonState) {
  //   // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
      if (moveButtonState == HIGH) {
        Serial.println("Motor On");
        enabled = true;
        currentSpeed = (current_direction == FORWARD ? MAX_SPEED : -MAX_SPEED);
        setSpeed(currentSpeed);
        state = RUN;
      } else {
        enabled = false;
        state = STOP;
        Serial.println("Motor Off");
      }

      lastMoveButtonState = moveButtonState;
      previousMillis[DEBOUNCE_INDEX_MOVE_BUTTON] = millis();
      Serial.println("");
    }
  }
}

void GatesOfMoria_Motor::report(String name) {
    float mSpeed = stepper.speed();
    Serial.print(name);
    Serial.print(" state: ");
    Serial.print(state);
    Serial.print(" speed: ");
    Serial.print(mSpeed);
    Serial.print("  pos: ");
    Serial.println(stepper.currentPosition());

}
