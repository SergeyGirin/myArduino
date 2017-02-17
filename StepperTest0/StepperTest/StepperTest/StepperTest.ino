// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>

// Connect a stepper motor with 48 steps per revolution (7.5 degree)
// to motor port #2 (M3 and M4)
AF_Stepper motor(1000, 1);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");

  motor.setSpeed(13);  // 10 rpm   
}

void loop() {
  Serial.println("Single coil steps");
  motor.step(200, FORWARD, SINGLE); 
  delay(1000);
  motor.step(200, BACKWARD, SINGLE); 
  delay(1000);
  motor.step(600, FORWARD, SINGLE); 
  delay(1000);
  motor.step(600, BACKWARD, SINGLE); 
  delay(1000);
  s90();
  
/*
  Serial.println("Double coil steps");
  motor.step(100, FORWARD, DOUBLE); 
  motor.step(100, BACKWARD, DOUBLE);

  Serial.println("Interleave coil steps");
  motor.step(100, FORWARD, INTERLEAVE); 
  motor.step(100, BACKWARD, INTERLEAVE); 

  Serial.println("Micrsostep steps");
  motor.step(100, FORWARD, MICROSTEP); 
  motor.step(100, BACKWARD, MICROSTEP); 
*/
}

void s90()
{
  motor.step(50, FORWARD, SINGLE); 
  delay(500);
  motor.step(50, FORWARD, SINGLE); 
  delay(500);
  motor.step(50, FORWARD, SINGLE); 
  delay(500);
  motor.step(50, FORWARD, SINGLE); 
  delay(500);
}

