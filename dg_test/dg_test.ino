#include <DigiCDC.h>



void setup() {
  SerialUSB.begin();
}


void loop() {
  //SerialUSB.println(F("<=== TEST ====>"));
  float x;
  float y;
  for (float i = 0; i <= 6.28; i += 0.1)
  {
    x = 30-30*cos(i);
    y = 30-30*sin(i);
    
      SerialUSB.print(i, DEC);
      SerialUSB.print(F(">"));
      SerialUSB.print(x, DEC);
      SerialUSB.print(F(":"));
      SerialUSB.println(y, DEC);
    
  }

  SerialUSB.delay(1000);
}

