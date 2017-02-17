#include <Ticker.h>
#include "TM1637.h"
#define TM1637_CLK  12
#define TM1637_DIO  14

// Прерывание
Ticker blinker;

TM1637 tm1637(TM1637_CLK, TM1637_DIO);
uint8_t brightness = 2;
bool          points   = true;
//uint8_t counter = 0;
int counter = 0;

void displayTm1637()
{
  brightness++;
  if (brightness>7) brightness=0;
  tm1637.set(brightness);
  int p0, p1, p2, p3;
  counter++;
  //if (counter >= 10000) counter = 0; 
  //p0 = counter/1000;
  if (counter >= 1000) counter = 0; 
  p0 = brightness;
  if (counter >= 1000) p1 = (counter - (counter / 1000)*1000) / 100; else p1 = counter/100;
  if (counter >= 100) p2 = (counter - (counter / 100)*100) / 10; else p2 = counter/10;
  points = !points;
  //tm1637.point(points);
  tm1637.display(0, p0);
  tm1637.display(1, p1);
  tm1637.display(2, p2);
  tm1637.display(3, counter % 10);
}

void timerIsr()
{
  displayTm1637();
}

void setup() {
  delay(1000);
  blinker.attach(0.5, timerIsr);
  // Инициализация дисплея
  tm1637.init();
  // Установка яркости дисплея
  tm1637.set(brightness);
}

void loop() {
  // put your main code here, to run repeatedly:

}
