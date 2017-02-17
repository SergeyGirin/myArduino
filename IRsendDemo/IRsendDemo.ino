/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include "IRremote.h"
const int IR_PIN = A0;


const int led = 13;

IRrecv irrecv(IR_PIN);
IRsend irsend;



void setup()
{
  Serial.begin(9600);
  //irsend.begin();
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  irrecv.enableIRIn();
}

void loop() {
  decode_results results;
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume();
  }
  //if (millis()/1000%2==0) digitalWrite(led, !digitalRead(led));
  if (millis()/1000%5==0) {
    digitalWrite(led, !digitalRead(led));
    //irsend.sendNEC(value, 32);
    int  myCod = 0x20DF10EF ;
    //irsend.send(NEC,myCod, 32);
    //irsend.sendNEC(myCod, 32);
    irsend.sendNEC(0x20DF10EF, 32);

    Serial.print("Send :");
    Serial.println(myCod,HEX);
    delay(1000);
  }
}
