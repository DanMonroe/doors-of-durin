#include <Arduino.h>

#include <GOM_Motor.h>

#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

const uint8_t stopButtonPin = 2; // Button to perform stop everything interrupt
volatile int stopButtonState = 0;     // current state of the stop button

const bool DEBUG = true;
const uint8_t motor2Pin_moveButton = 9;
const uint8_t motor2Pin_directionToggle = 4;
const uint8_t motor2Pin_runningLEDPin = 7;

elapsedMillis printTime;

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

// AccelStepper GOM_Astepper1(forwardstep1, backwardstep1);
AccelStepper GOM_Astepper2(forwardstep2, backwardstep2);

// GOM_Motor::GOM_Motor(bool DEBUG, AccelStepper _stepper, int motorIndex, uint8_t _moveButtonPin, uint8_t _directionTogglePin, uint8_t _motorRunningLEDPin) {
// GOM_Motor motor1 = GOM_Motor(DEBUG, GOM_Astepper1, 0, 
//   motor2Pin_moveButton, 
//   motor2Pin_directionToggle, 
//   motor2Pin_runningLEDPin
// );
GOM_Motor motor2 = GOM_Motor(DEBUG, GOM_Astepper2, 0, 
  motor2Pin_moveButton, 
  motor2Pin_directionToggle, 
  motor2Pin_runningLEDPin
);

// Interrupt when the STOP button is pressed
// Set state on all motors to STOP
void stopEverything() {
  Serial.println("STOP!");
  // motor1.stopEverything("Motor 1");
  motor2.stopEverything("Motor 2");
}

void checkStopButton() {
  stopButtonState = digitalRead(stopButtonPin);
  if (stopButtonState == HIGH) {
    stopEverything();
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
  
  AFMS.begin();

  // // setup motors
  // motor1.setupMotor();
  motor2.setupMotor();
}

void loop() {
  // motor1.run();
  motor2.run();

  // if (printTime >= 5000) {
  //   printTime = 0;
  //   // motor1.report("Motor 1");
  //   motor2.report("Motor 2");
  // } 
}