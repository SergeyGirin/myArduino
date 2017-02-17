#include "IRremote.h"
#include <Servo.h> 
 
//вход ик приёмника к А0
const int IR_PIN = A0;
int k = 0;
 
//создаём объект ик приёмника
IRrecv irrecv(IR_PIN);

Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
 
int pos = 0;    // variable to store the servo position 



void setup (){
 Serial.begin(9600);
 Serial.println("ready");
 //начинаем прослушивание ик сигналов
 irrecv.enableIRIn();
 myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
 myservo.write(pos);
}
 
void loop() {
 //в results будут помещаться принятые и 
 //декодированные ик команды
 decode_results results;
 //Если ик команда принята и успешно декодирована - 
 //выводим полученный код в сириал монитор
 if (irrecv.decode(&results)) {
  Serial.println(results.value, HEX);
  // коды кнопок
  switch (results.value) {
    case 0x2ADE08A3:
      k = 1;
      break;
    case 0x20DF8877:
      k = 1;
      break;    
    case 0x20DF48B7:
      k = 2;
      break;
    case 0x7F613A3B:
      k = 2;
      break;
    case 0x42C7C05B:
      k = 2;
      break;
    case 0x20DF40BF:
      k = 21;
      break;
    case 0x9E0A10FF:
      k = 21;
      break;      
    case 0xC3DAC5B2:
      k = 21;
      break;          
    case 0x8EEF4B83:
      k = 22;
      break;
    case 0x20DFC03F:
      k = 22;
      break;
    case 0xE7D0D303:
      k = 7;
      break;
    case 0x20DFE817:
      k = 7;
      break;
      
      
  }
  Serial.println(k);
  switch (k) {
    case 21:
      run(pos+10);
    break;
    case 22:
    run(pos-10); 
    break;    
    case 1:
      run(0);    
    break;    
    case 2:
      run(90);   
    break;    
    case 7:
      run(180); 
    break;    
    
  }
  Serial.println(pos);
  k = 0;  
  irrecv.resume();
  
 }
}
// процедура медленного поворота
void run(int n_pos)
{
  Serial.println("RUN");
  Serial.println(n_pos);
  if (n_pos>pos) {
    for (int i=pos; i <= n_pos; i++){
      pos = i;
      myservo.write(pos);
      delay(30);
    }
  } else {
    for (int i=pos; i >= n_pos; i--){
      pos = i;
      myservo.write(pos);
      delay(30);
    }    
  }
  if (pos>180) pos = 180;
  if (pos<0) pos = 0;
  Serial.println(pos);
  
}

