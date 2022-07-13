#ifndef _GatesOfMoria_Motor_h
#define _GatesOfMoria_Motor_h

#include <AccelStepper.h>

class GatesOfMoria_Motor {
  private:
    AccelStepper stepper;
    uint8_t moveButtonPin;    // the number of the pushbutton pin
    uint8_t directionTogglePin; // direction either forward or backwards
    uint8_t current_direction = 0;
    uint8_t directionToggleState = 0;         // current state of the direction toggle
    uint8_t lastDirectionToggleState = 0;
    uint8_t moveButtonState = 0;         // current state of the move button
    uint8_t lastMoveButtonState = 0;
    int currentSpeed = 0;
    unsigned long previousMillis[1];
    bool enabled = false;
    void setSpeed(int speed);
    void setState();

  public:
    GatesOfMoria_Motor(AccelStepper stepper, uint8_t moveButtonPin, uint8_t directionTogglePin);
    void setupMotor();
    void report(String name);
    void run();

};

#endif