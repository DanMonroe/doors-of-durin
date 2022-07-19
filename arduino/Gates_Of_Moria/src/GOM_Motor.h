#ifndef _GOM_Motor_h
#define _GOM_Motor_h

#include <Arduino.h>
#include <AccelStepper.h>

/**
 * @brief Gates of Moria Motor class
 * 
 */
class GOM_Motor {
  private:
    bool DEBUG = false;
    AccelStepper stepper;
    uint8_t motorRunningLEDPin;    // the number of the pushbutton pin
    uint8_t moveButtonPin;    // the number of the pushbutton pin
    uint8_t directionTogglePin; // direction either forward or backwards
    uint8_t current_direction = 0;
    uint8_t directionToggleState = 0;         // current state of the direction toggle
    uint8_t lastDirectionToggleState = 0;
    uint8_t moveButtonState = 0;         // current state of the move button
    uint8_t lastMoveButtonState = 0;
    
    uint8_t closedLimitSwitchPin;
    uint8_t closedLimitSwitchState = 0;
    uint8_t lastClosedLimitSwitchState = 0;

    int currentSpeed = 0;
    unsigned long previousMillis[2];

    static void rising();
    void println(String val);
    void print(String val);
    void println(long val);
    void print(long val);
    // void println(float val);
    // void print(float val);
    void setSpeed(int speed);
    void setState();
    void toggleRunningLEDIfNeeded();

  public:
    GOM_Motor(bool debug, AccelStepper stepper, uint8_t closedLimitSwitchPin, uint8_t moveButtonPin, uint8_t directionTogglePin, uint8_t _motorRunningLEDPin);
    void report(String name);
    void run();
    void setupMotor();
    void stopEverything(String name);

};

#endif