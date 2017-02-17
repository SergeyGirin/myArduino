// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>

// Connect a stepper motor with 48 steps per revolution (7.5 degree)
// to motor port #2 (M3 and M4)
AF_Stepper motor(1000, 1);
int pause = 200;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");

  motor.setSpeed(25);  // 10 rpm   
}

void loop() {
  r_begin();
  r_stop();
  
  motor.setSpeed(25);
  
  motor.step(1600, BACKWARD, INTERLEAVE); 
  delay(pause);
  s30();
  motor.setSpeed(9);
  s45();
  motor.setSpeed(25);
  s90();
  s180();
  r180();
  r90();
  motor.setSpeed(9);
  r45();
  motor.setSpeed(25);
  r30();
  
  /*
  Serial.println("Single coil steps");
  motor.step(100, FORWARD, SINGLE); 
  motor.step(100, BACKWARD, SINGLE); 

  Serial.println("Double coil steps");
  motor.step(100, FORWARD, DOUBLE); 
  motor.step(100, BACKWARD, DOUBLE);
*/
  Serial.println("Interleave coil steps");
  motor.step(1600, FORWARD, INTERLEAVE); 
  delay(pause);
  
  
/*
  Serial.println("Micrsostep steps");
  motor.step(100, FORWARD, MICROSTEP); 
  motor.step(100, BACKWARD, MICROSTEP); 
  */
}

void s90() {
  
  for (uint8_t i=4; i!=0; i--) {
    motor.step(100, FORWARD, INTERLEAVE); 
    delay(pause);
 }
  
}

void r90() {
  
  for (uint8_t i=4; i!=0; i--) {
    motor.step(100, BACKWARD, INTERLEAVE); 
    delay(pause);
 }
  
}

void s45() {
  
  for (uint8_t i=8; i!=0; i--) {
    motor.step(50, FORWARD, INTERLEAVE); 
    delay(pause);
 }
  
}

void r45() {
  
  for (uint8_t i=8; i!=0; i--) {
    motor.step(50, BACKWARD, INTERLEAVE); 
    delay(pause);
 }
  
}

void s180() {
  for (uint8_t j=2; j!=0; j--) {
    for (uint8_t i=2; i!=0; i--) {
      motor.step(200, FORWARD, INTERLEAVE); 
      delay(pause);
    }
  }
}

void r180() {
  for (uint8_t j=2; j!=0; j--) {
    for (uint8_t i=2; i!=0; i--) {
      motor.step(200, BACKWARD, INTERLEAVE); 
      delay(pause);
    } 
  }
}

void s30() {
  
  for (uint8_t i=18; i!=0; i--) {
    motor.step(25, FORWARD, INTERLEAVE); 
    delay(pause/2);
 }
  
}

void r30() {
  
  for (uint8_t i=18; i!=0; i--) {
    motor.step(25, BACKWARD, INTERLEAVE); 
    delay(pause/2);
 }
  
}

void r_stop() {
  
  for (uint8_t i=25; i!=0; i--) {
    motor.setSpeed(i);
    motor.step(50, BACKWARD, INTERLEAVE); 
    
 }
 delay(pause*4);
  
}

void r_begin() {
  
  for (uint8_t i=1; i!=26; i++) {
    motor.setSpeed(i);
    motor.step(50, BACKWARD, INTERLEAVE); 
    
 }
 motor.step(1200, BACKWARD, INTERLEAVE); 
 //delay(pause);
  
}

