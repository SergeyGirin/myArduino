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

// релюха
int pin_rele = 5;

//создаём объект ик приёмника
IRrecv irrecv(IR_PIN);

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

// исходное состояние сервы
int pos = 180;    // variable to store the servo position

// время для таймаута (возврат в исходное положение)
// 10 минут
long time_limit = long(10) * long(60) * long(1000);
long time_limit7 = 0;

unsigned long time_start;



void setup () {
  pinMode(pin_led, OUTPUT);
  pinMode(pin_rele, OUTPUT);
  Serial.begin(9600);
  Serial.println("ready");
  Serial.println(time_limit);
  //начинаем прослушивание ик сигналов
  irrecv.enableIRIn();
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  time_start = millis();
  time_led = millis();
  digitalWrite(pin_rele, LOW);
  myservo.write(pos);
  delay(500);
  digitalWrite(pin_rele, HIGH);
  s = int(EEPROM.read(0));
  if (s == 255) s = 180;
  Serial.print("Read (");
  Serial.print(s);
  Serial.println(")");
  run(s);

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
      case 0xD130B09F:
        k = 0;
        break;
      case 0x1A46B33F:
        k = 3;
        break;
      case 0x20DFC837:
        k = 3;
        break;
      case 0x9F3E1063:
        k = 4;
        break;
      case 0x2FB2625F:
        k = 5;
        break;
      case 0x4CEB59FF:
        k = 6;
        break;
      case 0x20DF6897:
        k = 6;
        break;
      case 0xC984587B:
        k = 8;
        break;
      case 0x2331B07F:
        k = 9;
        break;
      case 0xFFFFFFFF:
        break;
      // samsung
      case 0xF2B862C4:
        k = 1;
        break;
      case 0xFC8C27A4:
        k = 2;
        break;
      case 0xA857C03F:
        k = 3;
        break;
      case 0x34FB828:
        k = 3;
        break;        
      case 0xA85720DF:
        k = 4;
        break;
      case 0xE90FD844:
        k = 4;
        break;   
      case 0xA857A05F:
        k = 5;
        break;
      case 0x589B8648:
        k = 5;
        break;      
      case 0xA857609F:
        k = 6;
        break;
      case 0x2F0207E0:
        k = 6;
        break;       
      case 0xA857E01F:
        k = 7;
        break;
      case 0x35C59864:
        k = 7;
        break;     
      case 0xFD6B90A4:
        k = 8;
        break;
      case 0xA857906F:
        k = 9;
        break;
      case 0x4C65BAC8:
        k = 0;
        break;
      case 0xA857827D:
        k = 22;
        break;
      case 0xA85742BD:
        k = 21;
        break;
      case 0xA857D22D:
        k = 22;
        break;
      case 0xA85732CD:
        k = 21;
        break;
        
      default:
        k = -1;
    }
    Serial.println(k);
    if ( x == 0 ) {
      switch (k) {
        case 22:
          run(pos + 10);
          break;
        case 21:
          run(pos - 10);
          break;
        case 7:
          run(0);
          break;
        case 6:
          run(30);
          break;
        case 5:
          run(60);
          break;
        case 4:
          run(100);
          break;
        case 3:
          run(150);
          break;
        case 2:
          run(180);
          break;
        case 0:
          x = 1;
          break;
      }
    } else {
      x = 0;
      Serial.print("k = ");
      Serial.println(k);
      switch (k) {
        case 2:
          s = 180;
          break;
        case 3:
          s = 150;
          break;
        case 4:
          s = 90;
          break;
        case 5:
          s = 60;
          break;
        case 6:
          s = 30;
          break;
        case 7:
          s = 0;
          break;
        case 0:
          x = 1;
          digitalWrite(pin_rele, LOW);
          delay(30);
          digitalWrite(pin_rele, HIGH);
          delay(30);
          digitalWrite(pin_rele, LOW);
          delay(30);
          digitalWrite(pin_rele, HIGH);
          break;
        case -1:
          x = 1;
          break;
      }
      if ( x == 0) {
        Serial.print("Save (");
        Serial.print(s);
        Serial.println(")");
        EEPROM.write(0, byte(s));
        run(s);
        if ( s == 0){
          s = 90;
          EEPROM.write(0, byte(s));
          Serial.println("7");
          time_limit7 = long(20) * long(60) * long(1000);
        }
      }
    }
    //Serial.println(pos);
    //k = -1;
    irrecv.resume();
  }
  // отключалка по таймауту
  if (( time_start + time_limit + time_limit7 ) < millis()) {
    Serial.print("TimeOut (");
    Serial.print(s);
    Serial.println(")");

    run(s);
    time_limit7 = 0;
  }
  // мигалка светодиодом
  if (time_led + time_limit_led < millis()) {
    digitalWrite(pin_led, !digitalRead(pin_led));
    time_led = millis();
  }
}
// процедура медленного поворота
void run(int n_pos)
{
  digitalWrite(pin_led, HIGH);
  digitalWrite(pin_rele, LOW);
  delay(30); // пауза на включение реле
  //Serial.print("RUN(");
  //Serial.print(n_pos);
  //Serial.println(")");
  if (n_pos > pos) {
    for (int i = pos; i <= n_pos; i++) {
      pos = i;
      myservo.write(pos);
      delay(30);
    }
  } else {
    for (int i = pos; i >= n_pos; i--) {
      pos = i;
      myservo.write(pos);
      delay(30);
    }
  }
  if (pos > 180) pos = 180;
  if (pos < 0) pos = 0;
  // дергаю серву назад
  if (pos == 180) {
    //  pos = 178;
    myservo.write(pos);
    // 3 секунды на возврат сервы излюбого положения
    //delay(500);
  }
  // устанавливаю время старта для таймайта
  time_start = millis();
  //Serial.println(time_start);
  // тушим индикацию и релюху

  digitalWrite(pin_led, LOW);
  digitalWrite(pin_rele, HIGH);

}


