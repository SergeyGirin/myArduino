
const int led1 = 2; 
/*
const int led2 = 16; // D2
const int led3 = 2; // D1
const int led4 = 15; // D1
const int led5 = 4; // D1
const int led6 = 5; // D1
const int led7 = 12; // D1
const int led8 = 13; // D1
const int led9 = 14; // D1
*/


// Прерывание
#include <Ticker.h>
Ticker blinker1;
//Ticker blinker2;
//Ticker blinker3;



void setup() {
  pinMode(led1, OUTPUT);
/*  
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(led8, OUTPUT);
  pinMode(led9, OUTPUT);
 */
 // pinMode(led10, OUTPUT);
  //pinMode(led11, OUTPUT);
  //pinMode(led12, OUTPUT);
  //pinMode(led13, OUTPUT);

  digitalWrite(led1, 0);
/*  
  digitalWrite(led2, 0);
  digitalWrite(led3, 0);
  digitalWrite(led4, 0);
  digitalWrite(led5, 0);
  digitalWrite(led6, 0);
  digitalWrite(led7, 0);
  digitalWrite(led8, 0);
  digitalWrite(led9, 0);
*/  
  //digitalWrite(led10, 0);
  //digitalWrite(led11, 0);
  //digitalWrite(led12, 0);
  //digitalWrite(led13, 0);
  
  Serial.begin(115200);
  Serial.println();
  // Прерывание 1 сек.
  blinker1.attach(1, timerIsr1);
  
}


  
void loop() {

}



void timerIsr1()
{
   digitalWrite(led1, !digitalRead(led1));
/*   
   digitalWrite(led2, !digitalRead(led2));
   digitalWrite(led3, !digitalRead(led3));
   digitalWrite(led4, !digitalRead(led4));
   digitalWrite(led5, !digitalRead(led5));
   digitalWrite(led6, !digitalRead(led6));
   digitalWrite(led7, !digitalRead(led7));
   digitalWrite(led8, !digitalRead(led8));
   digitalWrite(led9, !digitalRead(led9));
*/   
   //digitalWrite(led10, !digitalRead(led10));
   //digitalWrite(led11, !digitalRead(led11));
   //digitalWrite(led12, !digitalRead(led12));
   //digitalWrite(led13, !digitalRead(led13));
}

int i = 0;
int timer = millis()/1000;
void count()
{
   if (timer == millis()/1000)
  {
    i++;
  } 
  else
  {
     Serial.print(millis()/1000);
     Serial.print(" i=");
     Serial.println(i);
     timer = millis()/1000;
     i = 0;
  }
}



