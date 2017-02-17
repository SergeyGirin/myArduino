#include "IRremote.h"
#include <Servo.h>
#include <EEPROM.h> //Needed to access the eeprom read write functions


/*
пин А0  - вход ИК датчика
пин Д9  - выход для сервы
пин Д13 - выход для мигалки
пин Д5  - выход для релюхи (релюха включает питание сервы, чтобы её можно было крутить руками)
*/

//вход ик приёмника к А0
const int IR_PIN = A0;
int k = -1;
int x = 0;
int s = 180;

// мигалка диодом на плате
int pin_led = 13;
unsigned long time_limit_led = 1000;
unsigned long time_led;

//создаём объект ик приёмника
IRrecv irrecv(IR_PIN);
IRsend irsend;

unsigned long time_start;



void setup () {
  pinMode(pin_led, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("ready");

  //начинаем прослушивание ик сигналов
  irrecv.enableIRIn();
  time_start = millis();
  time_led = millis();

  Serial.print("Read (");
  Serial.print(s);
  Serial.println(")");
  

}

void loop() {

  decode_results results;
  //Если ик команда принята и успешно декодирована -
  //выводим полученный код в сириал монитор
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume();
  }

  // мигалка светодиодом
  if (time_led + time_limit_led < millis()) {
    digitalWrite(pin_led, !digitalRead(pin_led));
    time_led = millis();
  }
  if (millis()/1000%5==0) 
  {
    Serial.println("Send :");
    irsend.sendNEC(0x20DF10EF, 32);
    delay(500);
    //20DF10EF
  }

}



