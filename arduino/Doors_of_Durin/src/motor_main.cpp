#include <Arduino.h>

#include "pins.h"
// #include <DOD_PixelStrips.h>
#include <Wire.h>
#include <DOD_Motor.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

const bool MOTORS_ENABLED = true;
const bool DEBUG = false;
// volatile int state;
int motorState = 0;
int wireState = 0;
int lastWireState = 0;

elapsedMillis printTime;

void receiveEvent() {
  // Read while data received
  while (0 < Wire.available()) {
    wireState = Wire.read();
    Serial.print("receiveEvent wireState ");
    Serial.println(wireState);

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

// Adafruit_StepperMotor *stepper1 = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *stepper2 = AFMS.getStepper(200, 2);

void forwardstep1() {
  stepper1->onestep(FORWARD, DOUBLE);
}
void backwardstep1() {
  stepper1->onestep(BACKWARD, DOUBLE);
}

void forwardstep2() {
  stepper2->onestep(FORWARD, DOUBLE);
}

void backwardstep2() {
  stepper2->onestep(BACKWARD, DOUBLE);
}


AccelStepper DOD_Astepper1(forwardstep1, backwardstep1);
AccelStepper DOD_Astepper2(forwardstep2, backwardstep2);

// startingDirection should be opposite for each motor;
DOD_Motor motor1 = DOD_Motor(
  DEBUG, 
  DOD_Astepper1, 
  // motor1Pin_closeOpenButton, 
  motor1Pin_closeLimitSwitch, 
  motor1Pin_moveButton, 
  motor1Pin_directionToggle, 
  motor1Pin_runningLED,
  1,
  motor1_targetOpenPosition
);
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



void setup() {

  // Initialize I2C communications as Slave
  Wire.begin(SLAVE_ADDR);
  
  // Function to run when data requested from master
  // Wire.onRequest(requestEvent); 
  
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);

  Serial.begin(9600);
  // // Serial.begin(115200);
  Serial.println("Start LED");
  Serial.println();

  if (MOTORS_ENABLED) {
    AFMS.begin();
    // // setup motors
    motor1.setupMotor();
    motor2.setupMotor();
  }
}

void loop() {
  if (MOTORS_ENABLED) {
    if (wireState == 200) {
      motor1.initiateAction(HIGH);
      motor2.initiateAction(HIGH);
      wireState = 0;
    }

    motor1.run();
    motor2.run();

    // TODO send motorState back to main to control sounds/leds
    // motorState = motor2.getState();
  }

  if (printTime >= 1000) {
    printTime = 0;
    if (MOTORS_ENABLED) {
      motor1.report("Motor 1");
      motor2.report("Motor 2");
    }
  } 

}
