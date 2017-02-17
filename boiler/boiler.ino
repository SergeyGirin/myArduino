#include "IRremote.h"
#include <Servo.h>


/*
пин А0  - вход ИК датчика
пин Д9  - выход для сервы
пин Д13 - выход для мигалки
пин Д5  - выход для релюхи (релюха включает питание сервы, чтобы её можно было крутить руками)
*/

//вход ик приёмника к А0
const int IR_PIN = A0;
int k = 0;

// мигалка диодом на плате
int pin_led = 13;
unsigned long time_limit_led = 100;
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
// 15 минут
long time_limit = long(15) * long(60) * long(1000);

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
  myservo.write(pos);
  time_start = millis();
  time_led = millis();
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
      case 22:
        run(pos + 10);
        break;
      case 21:
        run(pos - 10);
        break;
      case 7:
        run(0);
        break;
      case 2:
        run(90);
        break;
      case 1:
        run(180);
        break;
    }
    Serial.println(pos);
    k = 0;
    irrecv.resume();
  }
  // отключалка по таймауту
  if (( time_start + time_limit ) < millis()) run(180);
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
  digitalWrite(pin_rele, HIGH);
  delay(30); // пауза на включение реле
  Serial.print("RUN(");
  Serial.print(n_pos);
  Serial.println(")");
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
  if (pos = 180) {
    pos = 178;
    myservo.write(pos);
  }
  // устанавливаю время старта для таймайта
  time_start = millis();
  Serial.println(time_start);
  // тушим индикацию и релюху
  digitalWrite(pin_led, LOW);
  digitalWrite(pin_rele, LOW);

}

