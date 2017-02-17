#include <DigiUSB.h>

float  x;
float  y;
float  r = 30;
float  Pi = 355/113;
float inc = 1/30;

void setup() {
  DigiUSB.begin();
  pinMode(1,OUTPUT);
}


void loop() {
  // print output
  DigiUSB.println("Waiting for input...");
  DigiUSB.println(inc);
  digitalWrite(1,!digitalRead(1));
  DigiUSB.delay(1000);
  //
}
