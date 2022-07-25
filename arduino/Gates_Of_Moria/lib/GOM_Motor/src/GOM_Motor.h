#ifndef _GOM_Motor_h
#define _GOM_Motor_h

#include <Arduino.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

#define IICADDRESS 0x60

/**
 * @brief Gates of Moria Motor class
 * 
 */
class GOM_Motor {
  private:
    bool DEBUG = true;
    bool initialized = false;

    AccelStepper stepper;
    uint8_t motorRunningLEDPin;           // the number of the pushbutton pin
    
    // uint8_t closeOpenButtonPin;                // the number of the close or open button pin
    // uint8_t closeOpenButtonState = 0;          // current state of the close or open button
    // uint8_t lastCloseOpenButtonPinState = 0;

    uint8_t moveButtonPin;                // the number of the manual move button pin
    uint8_t moveButtonState = 0;          // current state of the manual move button
    uint8_t lastMoveButtonState = 0;

    uint8_t directionTogglePin;           // direction either forward or backwards
    uint8_t directionToggleState = 0;     // current state of the direction toggle
    uint8_t lastDirectionToggleState = 0;
    
    uint8_t closeLimitSwitchPin;          // close limit switch
    uint8_t closeLimitSwitchState = 0;
    uint8_t lastCloseLimitSwitchState = 0;

    int closingDirection = 0;
    int currentDirection = 0;
    int currentSpeed = 0;
    int openingDirection = 0;
    int targetOpenPosition = 0;

    int currentPosition = 0;

    void println(String val);
    void print(String val);
    void println(long val);
    void print(long val);
    void setSpeed(int speed);
    void setState();
    void toggleRunningLEDIfNeeded();
    void toggleDirection();

    //Adafruit Motor Shield object
	  // Adafruit_MotorShield AFMS; 
	  // //Adafruit Stepper Motor object
	  // Adafruit_StepperMotor *AFstepper; 
	  // //AccelStepper wrapper
	  // AccelStepper* stepper; 

  public:
// uint8_t closeOpenButtonPin, 
    GOM_Motor(
      bool debug, 
      AccelStepper stepper, 
      uint8_t closeLimitSwitchPin, 
      uint8_t moveButtonPin, 
      uint8_t directionTogglePin, 
      uint8_t motorRunningLEDPin, 
      int closingDirection,
      long targetOpenPosition
    );
    void report(String name);
    void run();
    void setupMotor();
    void stopEverything(String name);
    void initiateAction(int actionButtonState);  // HIGH or LOW

    // void forwardstep();
	  // void (*fwdstp)();
	  // void backwardstep();
	  // void(*bckwdstp)();

};

#endif