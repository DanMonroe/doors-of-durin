#include <Arduino.h>

#include "pins.h"
#include <DOD_PixelStrips.h>
#include <Wire.h>

// Define string with response to Master
String answer = "Hello";

const bool LEDS_ENABLED = true;
volatile int state;

DOD_PixelStrips *strips = new DOD_PixelStrips();

// Wire.onReceive(handler);

// void handler(int howMany) {
//     byte message = Wire.read();
// }

void receiveEvent() {
 
  // Read while data received
  while (0 < Wire.available()) {
    state = Wire.read();
  }
  
  // Print to Serial Monitor
  Serial.println("Receive event");
}
 
// void requestEvent() {
 
//   // Setup byte variable in the correct size
//   byte response[ANSWERSIZE];
  
//   // Format answer as array
//   for (byte i=0;i<ANSWERSIZE;i++) {
//     response[i] = (byte)answer.charAt(i);
//   }
  
//   // Send response back to Master
//   Wire.write(response,sizeof(response));
  
//   // Print to Serial Monitor
//   Serial.println("Request event");
// }

void setup() {

  // Initialize I2C communications as Slave
  Wire.begin(SLAVE_ADDR);
  
  // Function to run when data requested from master
  // Wire.onRequest(requestEvent); 
  
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);

  Serial.begin(9600);
  // // Serial.begin(115200);
  Serial.println("Start LED");
  Serial.println();

  if (LEDS_ENABLED) {
    strips->setupStrips();
  }

}

void loop() {
  // delay(50);
  
  if (LEDS_ENABLED) {
    strips->loop();
  }
}
