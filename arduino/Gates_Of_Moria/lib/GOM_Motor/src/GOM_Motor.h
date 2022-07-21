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
    bool DEBUG = false;
    AccelStepper stepper;
    unsigned int motorIndex;              // which motor index - used for pin arrays in .cpp
    uint8_t motorRunningLEDPin;           // the number of the pushbutton pin
    uint8_t moveButtonPin;                // the number of the pushbutton pin
    uint8_t directionTogglePin;           // direction either forward or backwards
    uint8_t current_direction = 0;
    uint8_t directionToggleState = 0;     // current state of the direction toggle
    uint8_t lastDirectionToggleState = 0;
    uint8_t moveButtonState = 0;          // current state of the move button
    uint8_t lastMoveButtonState = 0;
    
    int currentSpeed = 0;

    static void closeLimitSwitchCallback();
    void setCloseLimitSwitchPin(uint8_t val);
    void println(String val);
    void print(String val);
    void println(long val);
    void print(long val);
    void setSpeed(int speed);
    void setState();
    void toggleRunningLEDIfNeeded();

    //Adafruit Motor Shield object
	  // Adafruit_MotorShield AFMS; 
	  // //Adafruit Stepper Motor object
	  // Adafruit_StepperMotor *AFstepper; 
	  // //AccelStepper wrapper
	  // AccelStepper* stepper; 

  public:
    GOM_Motor(bool debug, AccelStepper stepper, int motorIndex, uint8_t moveButtonPin, uint8_t directionTogglePin, uint8_t _motorRunningLEDPin);
    void report(String name);
    void run();
    void setupMotor();
    void stopEverything(String name);

    // void forwardstep();
	  // void (*fwdstp)();
	  // void backwardstep();
	  // void(*bckwdstp)();

};

#endif