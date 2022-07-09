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

const int buttonPin = 2;    // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

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

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  AFMS.begin();

  Astepper1.setMaxSpeed(2000);
  Astepper1.setAcceleration(500);
  Astepper1.setSpeed(2000);
  // Astepper1.moveTo(pos);
}

// int i;
void loop() {
    // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // compare the buttonState to its previous state
  // if (buttonState != lastButtonState) {
    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (buttonState == HIGH) {
      // turn LED on:
      digitalWrite(ledPin, HIGH);
      Serial.println("On");

      Astepper1.move(200);
      Astepper1.run();

    } else {
      // turn LED off:
      digitalWrite(ledPin, LOW);
      Serial.println("Off");
      
      Astepper1.stop();
    }
    // delay(50);
  // }


  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
    
  // if (Astepper1.distanceToGo() == 0) {
  //   delay(500);
  //   pos = -pos;
  //   Astepper1.moveTo(pos);
  // }
  // Astepper1.run();
}