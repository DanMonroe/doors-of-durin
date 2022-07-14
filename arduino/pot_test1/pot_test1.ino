#include <elapsedMillis.h>

// #define speedPin = A0;
#define MIN_SPEED 1
#define MAX_SPEED 5000
// #define POT_DIFFERENCE_RANGE = 6;

// #define MIDDLE_RANGE = 200;

int potValue = 0;
// int potPreviousValue = 0;

int mappedValue = 0;

elapsedMillis potReadTime;

void setup() {
  Serial.begin(115200);

  // pinMode(potPin, OUTPUT);
}

void loop() {
  if (potReadTime >= 1500) {
    potReadTime = 0;

    potValue = analogRead(A0);
  
    Serial.print(potValue);

    if((potValue < 611) && (potValue > 411)) {
      // MIddle - off
      Serial.println("   Motor off");
    } else if (potValue > 610) {
      mappedValue = map(potValue, 611, 1023, MIN_SPEED, MAX_SPEED);
      Serial.print("   left  ");
      Serial.println(mappedValue);
    } else {
      mappedValue = map(potValue, 0, 411, MAX_SPEED, MIN_SPEED);
      Serial.print("   right ");
      Serial.println(mappedValue);
    }
  } 
}
