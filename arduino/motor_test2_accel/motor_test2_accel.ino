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

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor *myStepper = AFMS.getStepper(200, 2);


void forwardstep1() {
  myStepper->onestep(FORWARD, DOUBLE);
}

void backwardstep1() {
  myStepper->onestep(BACKWARD, DOUBLE);
}

AccelStepper Astepper1(forwardstep1, backwardstep1);

int pos = 1000;
void setup() {
  // while (!Serial);
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("setup");

  AFMS.begin();
   
  Astepper1.setMaxSpeed(2000);
  Astepper1.setAcceleration(500);
  Astepper1.setSpeed(2000);
  Astepper1.moveTo(pos);
}

// int i;
void loop() {
  if (Astepper1.distanceToGo() == 0) {
    delay(500);
    pos = -pos;
    Astepper1.moveTo(pos);
  }
  Astepper1.run();
}