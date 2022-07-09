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

const int moveButtonPin = 2;    // the number of the pushbutton pin
const int directionTogglePin = 4; // direction either forward or backwards
const int ledPin =  13;      // the number of the LED pin
const int DIRECTION_FORWARD =  1;
const int DIRECTION_BACKWARD =  -1;

int moveButtonState = 0;         // current state of the move button
int lastMoveButtonState = 0;     // previous state of the move button

int current_direction = 0;
int directionToggleState = 0;         // current state of the direction toggle
int lastDirectionToggleState = 0;     // previous state of the direction toggle

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

const int FORWARD_SPEED = -1;

void setup() {
  // while (!Serial);
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("setup4");

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(moveButtonPin, INPUT);
  pinMode(directionTogglePin, INPUT);

  directionToggleState = digitalRead(directionTogglePin);
  if (directionToggleState == HIGH) {
    Serial.println("Toggle On");
    current_direction = DIRECTION_FORWARD;
  } else {
    Serial.println("Toggle Off");
    current_direction = DIRECTION_BACKWARD;
  }
  Serial.print("Initial current direction ");
  Serial.println(current_direction);


  // current_direction = DIRECTION_FORWARD;

  // AFMS.begin();

  // Astepper1.setMaxSpeed(2000);
  // Astepper1.setAcceleration(500);
  // Astepper1.setSpeed(2000);
  // Astepper1.moveTo(pos);
}

// int i;
void loop() {
    // read the state of the pushbutton value:
  // moveButtonState = digitalRead(moveButtonPin);

  // if (Astepper1.distanceToGo() == 0) {

  directionToggleState = digitalRead(directionTogglePin);

  if (directionToggleState != lastDirectionToggleState) {
    if (directionToggleState == HIGH) {
      Serial.println("Toggle On");
      current_direction = DIRECTION_FORWARD;
    } else {
      Serial.println("Toggle Off");
      current_direction = DIRECTION_BACKWARD;
    }
    Serial.print("current direction ");
    Serial.println(current_direction);
    delay(100);
  }
  lastDirectionToggleState = directionToggleState;

  moveButtonState = digitalRead(moveButtonPin);
  // // compare the buttonState to its previous state
  if (moveButtonState != lastMoveButtonState) {
  //   // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (moveButtonState == HIGH) {
  //     // turn LED on:
  //     digitalWrite(ledPin, HIGH);
      Serial.println("Motor On");
      Serial.print(" Moving ");
      Serial.println(current_direction == DIRECTION_FORWARD ? "FORWARD" : "BACKWARD");
  //     Serial.println(Astepper1.distanceToGo());

  //     // Astepper1.move(FORWARD_SPEED);
  //     Astepper1.moveTo(-100);

  //     Astepper1.run();

    } else {
  //     // turn LED off:
  //     digitalWrite(ledPin, LOW);
      Serial.println("Motor Off");
      
  //     Astepper1.stop();
    }
    delay(50);
    Serial.println("");
  }


  // // save the current state as the last state, for next time through the loop
  lastMoveButtonState = moveButtonState;
  
    
  // if (Astepper1.distanceToGo() == 0) {
    // delay(500);
    // pos = current_direction * pos;
    // // pos = -pos;
    // Serial.print("moveTo pos:");
    // Serial.println(pos);
    // Astepper1.moveTo(pos);
  // } if (Astepper1.distanceToGo() == 0)
  // Astepper1.run();
}