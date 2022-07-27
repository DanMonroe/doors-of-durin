#ifndef _DOD_Sound_h
#define _DOD_Sound_h

#include <Arduino.h>

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

class DOD_Sound {
  private:
    void printDirectory(File dir, int numTabs);

  public:
    DOD_Sound();
    void setup();
    void run();
};

#endif