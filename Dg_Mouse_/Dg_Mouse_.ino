// DigiMouse test and usage documentation
// CAUTION!!!! This does click things!!!!!!!!
// Originally created by Sean Murphy (duckythescientist)

#include <DigiMouse.h>
#include "DigiKeyboard.h"

void setup() {
  DigiMouse.begin(); //start or reenumerate USB - BREAKING CHANGE from old versions that didn't require this
}


void loop() {
  float x;
  float y;
  for (float i = 0; i <= 6.28318530718; i += 0.01)
  {
    x = 4*cos(i);
    y = 4*sin(i);
    //DigiMouse.moveX(x);
    //DigiMouse.moveY(y);
    DigiMouse.move(x,y,0);
    DigiMouse.delay(2);
  }
  DigiMouse.moveX(-x/4);
  DigiMouse.delay(3000);
}
