#include <Arduino.h>

#include <DOD_Motor.h>

#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

const uint8_t stopButtonPin = 2; // Button to perform stop everything interrupt
volatile int stopButtonState = 0;     // current state of the stop button
const uint8_t initiateActionButtonPin = 10; 
uint8_t initiateActionButtonState = 0;          // current state of the action button
uint8_t lastInitiateActionButtonState = 0;

const bool DEBUG = false;
const int debounceTime = 200;
const uint8_t motor2Pin_directionToggle = 4;
const uint8_t motor2Pin_runningLED = 7;
const uint8_t motor2Pin_closeLimitSwitch = 8;
const uint8_t motor2Pin_moveButton = 9;
const long motor2_targetOpenPosition = 500;  // eventually 7000

// const uint8_t motor1Pin_directionToggle = 4;
// const uint8_t motor1Pin_runningLED = 7;
// const uint8_t motor1Pin_closeOpenButton = 10;
// const uint8_t motor1Pin_closeLimitSwitch = 8;
// const uint8_t motor1Pin_moveButton = 9;

elapsedMillis printTime;
elapsedMillis lastInitiateActionTime;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Adafruit_StepperMotor *stepper1 = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *stepper2 = AFMS.getStepper(200, 2);

// void forwardstep1() {
//   stepper1->onestep(FORWARD, DOUBLE);
// }

// void backwardstep1() {
//   stepper1->onestep(BACKWARD, DOUBLE);
// }

void forwardstep2() {
  stepper2->onestep(FORWARD, DOUBLE);
}

void backwardstep2() {
  stepper2->onestep(BACKWARD, DOUBLE);
}


// AccelStepper DOD_Astepper1(forwardstep1, backwardstep1);
AccelStepper DOD_Astepper2(forwardstep2, backwardstep2);

// startingDirection should be opposite for each motor;
// DOD_Motor motor1 = DOD_Motor(
//   DEBUG, 
//   DOD_Astepper1, 
//   motor1Pin_closeOpenButton, 
//   motor1Pin_closeLimitSwitch, 
//   motor1Pin_moveButton, 
//   motor1Pin_directionToggle, 
//   motor1Pin_runningLED,
//   1
// );
DOD_Motor motor2 = DOD_Motor(
  DEBUG, 
  DOD_Astepper2, 
  // motor2Pin_closeOpenButton, 
  motor2Pin_closeLimitSwitch, 
  motor2Pin_moveButton, 
  motor2Pin_directionToggle, 
  motor2Pin_runningLED,
  -1,
  motor2_targetOpenPosition
);

// Interrupt when the STOP button is pressed
// Set state on all motors to STOP
void stopEverything() {
  Serial.println("STOP!");
  // motor1.stopEverything("Motor 1");
  motor2.stopEverything("Motor 2");
  stepper2->release();
}

void checkStopButton() {
  stopButtonState = digitalRead(stopButtonPin);
  if (stopButtonState == HIGH) {
    stopEverything();
  }
}

// When events happen, here is where we communicate to the
// motors, leds, servos, sound, etc.
// called from the main loop()
void checkInitiateActionButton() {
    if ( lastInitiateActionTime >= debounceTime) {
    initiateActionButtonState = digitalRead(initiateActionButtonPin);
    if (initiateActionButtonState != lastInitiateActionButtonState) {
      // motor1.initiateAction(initiateActionButtonState);
      motor2.initiateAction(initiateActionButtonState);
      lastInitiateActionButtonState = initiateActionButtonState;
      lastInitiateActionTime = 0;
    }
  }
}

void setup() {
  // if (DEBUG) {
    Serial.begin(9600);
    // Serial.begin(115200);
    Serial.println("Start");
    Serial.println();
  // }

  // Setup Emergency STOP button  
  pinMode(stopButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(stopButtonPin), checkStopButton, RISING);
  
  pinMode(initiateActionButtonPin, INPUT);

  AFMS.begin();

  // DOD_Astepper2.setMaxSpeed(6000);
  // DOD_Astepper2.setSpeed(6000);

  // // setup motors
  // motor1.setupMotor();
  motor2.setupMotor();
}

void loop() {
  checkInitiateActionButton();

  // motor1.run();
  motor2.run();

  if (printTime >= 1000) {
    printTime = 0;
  //   // motor1.report("Motor 1");
    motor2.report("Motor 2");
  } 
}