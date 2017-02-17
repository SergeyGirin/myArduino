
//#include <DigiUSB.h>
#include <DigiCDC.h>

//#include <DigiMouse.h>
//#include <SoftSerial.h>


float  x;
float  y;
//int  r = 30;
//float  Pi = 355/113;
//float inc ;


void setup() {
  pinMode(1,OUTPUT);
  //DigiMouse.begin(); //start or reenumerate USB - BREAKING CHANGE from old versions that didn't require this
  SerialUSB.begin();
  
}

void loop() {
  // If not using plentiful DigiMouse.delay(), make sure to call
  // DigiMouse.update() at least every 50ms
  
  // move across the screen
  // these are signed chars
  //Pi = 355/113;
  //inc = 1/30;
  //delay(1000);
  
//  SerialUSB.println(inc);
  //SerialUSB.println();
for (int i=0;i<=4;i++)
  {
    digitalWrite(1,!digitalRead(1));
    SerialUSB.print(i);
    SerialUSB.print(">");
    x =  30 * cos (i);
    y =  30 * sin (i);
 // DigiMouse.moveY(y);
//  DigiMouse.moveX(x);
//  DigiMouse.delay(500);
  SerialUSB.print(x);
  SerialUSB.print(":");
  SerialUSB.println(y);
  //delay(500);
  }
  //SerialUSB.println("======");
  delay(10000);
  /*
  // or DigiMouse.move(X, Y, scroll) works
  
  // three buttons are the three LSBs of an unsigned char
  DigiMouse.setButtons(1<<0); //left click
  DigiMouse.delay(500);
  DigiMouse.setButtons(0); //unclick all
  DigiMouse.delay(500);

  //or you can use these functions to click
  DigiMouse.rightClick();
  DigiMouse.delay(500);
  DigiMouse.leftClick();
  DigiMouse.delay(500);
  DigiMouse.middleClick();
  DigiMouse.delay(500);

  //for compatability with other libraries you can also use DigiMouse.move(X, Y, scroll, buttons)
  */
}
