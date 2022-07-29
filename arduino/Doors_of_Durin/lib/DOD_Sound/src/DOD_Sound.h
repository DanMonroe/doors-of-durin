#ifndef _DOD_Sound_h
#define _DOD_Sound_h

#include <Arduino.h>

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

class DOD_Sound {
  private:
    bool DEBUG = false;
    void playSound(const char * soundName);
    void printDirectory(File dir, int numTabs);
    void stopIfPlaying();
    int lastMotorState = 0;

    void println(String val);
    void print(String val);
    void println(long val);
    void print(long val);
    void println(uint32_t, int);


  public:
    DOD_Sound(bool debug);
    void setup();
    void run(int motorState);
};

#endif