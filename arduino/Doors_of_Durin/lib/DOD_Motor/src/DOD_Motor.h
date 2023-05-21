#ifndef _DOD_Motor_h
#define _DOD_Motor_h

#include <Arduino.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

#define IICADDRESS 0x60

/**
 * @brief Gates of Moria Motor class
 * 
 */
class DOD_Motor {
  private:
    // bool DEBUG = false;
    // bool initialized = false;
    // String name = "";

    // AccelStepper stepper;
    /**
     * 
    // uint8_t motorRunningLEDPin;           // the number of the pushbutton pin
    // uint8_t closeOpenButtonPin;                // the number of the close or open button pin
    // uint8_t closeOpenButtonState = 0;          // current state of the close or open button
    // uint8_t lastCloseOpenButtonPinState = 0;
    // uint8_t moveButtonPin;                // the number of the manual move button pin
    // uint8_t moveButtonState = 0;          // current state of the manual move button
    // uint8_t lastMoveButtonState = 0;
    // uint8_t directionTogglePin;           // direction either forward or backwards
    // uint8_t directionToggleState = 0;     // current state of the direction toggle
    // uint8_t lastDirectionToggleState = 0;
    */
    
    // uint8_t closeLimitSwitchPin;          // close limit switch
    // uint8_t closeLimitSwitchState = 0;
    // uint8_t lastCloseLimitSwitchState = 0;

    // int closingDirection = 0;
    // int currentDirection = 0;
    // int currentSpeed = 0;
    // int openingDirection = 0;
    // int targetOpenPosition = 0;

    // int currentPosition = 0;

    // void println(String val);
    // void print(String val);
    // void println(long val);
    // void print(long val);
    // void setSpeed(int speed);
    // void setState();
    // void toggleRunningLEDIfNeeded();
    // void toggleDirection();

  public:
    // DOD_Motor(
    //   String name,
    //   bool debug, 
    //   AccelStepper stepper, 
    //   uint8_t closeLimitSwitchPin, 
    //   int closingDirection,
    //   long targetOpenPosition
    // );
    // void report();
    // void run();
    // void setupMotor();
    // void stopEverything();
    // void initiateAction(int actionButtonState);  // HIGH or LOW
    // int  getState();

};

#endif