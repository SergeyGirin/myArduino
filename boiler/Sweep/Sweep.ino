/* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/Sweep
*/ 

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
 
int pos = 0;    // variable to store the servo position 
int steper =160;
 
void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  pos = 0;
  myservo.write(pos);
  delay(500);
} 
 
 
void loop() 
{ 
  
  if (pos>=160)
  {                                  // in steps of 1 degree 
    steper = -steper;
  } 
  if ( pos<=0 && steper<0)
  {                  
    steper = -steper;
  }
  pos += steper;  
  myservo.write(pos);              // tell servo to go to position in variable 'pos' 
  delay(1000);                       // waits 15ms for the servo to reach the position 
  
} 

