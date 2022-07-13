/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/

#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <elapsedMillis.h>

#include "GatesOfMoria_Motor.h"

// const int ledPin =  13;      // the number of the LED pin


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

// AccelStepper Astepper1(forwardstep1, backwardstep1);

// AccelStepper GOM_Astepper1(forwardstep1, backwardstep1);
AccelStepper GOM_Astepper2(forwardstep2, backwardstep2);

// GatesOfMoria_Motor motor1 = GatesOfMoria_Motor(GOM_Astepper1, 2, 4);
GatesOfMoria_Motor motor2 = GatesOfMoria_Motor(GOM_Astepper2, 2, 4);


elapsedMillis printTime;

void setup() {
  // while (!Serial);
  Serial.begin(115200);

  AFMS.begin();

  // setup motors
  // motor1.setupMotor();
  motor2.setupMotor();
}


void loop() {

  // motor1.run();
  motor2.run();

  if (printTime >= 5000) {
    printTime = 0;
    // motor1.report("Motor 1");
    motor2.report("Motor 2");
  } 
}