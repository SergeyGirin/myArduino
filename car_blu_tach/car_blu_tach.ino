
#include <SoftwareSerial.h>

int incomingByte = 0;   // переменная для хранения полученного байта
int key_move = 0;
unsigned long key_time = 0;
unsigned long speed_time = 0;
boolean send = false;


int led = 13;
float i = 0;
float b = 50000;
float c = 48;
int a = 0;
uint8_t speed = 255; //максимальная скорость по умолчанию
uint8_t speed_k = 45;
int speed_temp = 3000;
int speed_move = 100;
int mLeft = 0;
int mRigth = 0;
String Message = "";

char s;
// время для таймаута (возврат в исходное положение)
// 15 минут
long time_limit =  long(3000);

unsigned long time_start;



#define init {D4_Out; D5_Out; D6_Out; D7_Out; D8_Out; D11_Out; D12_Out;} 





SoftwareSerial blue(2,3);


void setup() {
    //Serial.begin(9600); // устанавливаем последовательное соединение
    Serial.begin(115200); // устанавливаем последовательное соединение
    pinMode(led, OUTPUT);
    blue.begin(9600);
    time_start = millis();
}
 
 
void loop() {
    // отключалка по таймауту
    if (( time_start + time_limit ) < millis()) myTimeout();
    
    if (Serial.available() > 0) 
    {  
      //если есть доступные данные
      time_start = millis();
      // считываем байт
      incomingByte = Serial.read();
      // отсылаем то, что получили
      Serial.print("I received: ");
    }
    if (blue.available() > 0) {  
      //если есть доступные данные
      // считываем байт
      incomingByte = blue.read();
 
      // отсылаем то, что получили
      //Serial.print("I received blue: ");
      //blue.print("I received blue: "); 
      //Serial.print(char(incomingByte));
      if (incomingByte==10) {
        Serial.print("Message: ");
        Serial.println(Message);
        //Serial.println(Message.substring(0,Message.indexOf(';',1)));
        //Serial.println(Message.substring(2,Message.indexOf(';',3)));
        //Serial.println(Message.substring(Message.indexOf(';',3)+1,Message.indexOf(';',Message.indexOf(';',3)+1)));
        mLeft = Message.substring(2,Message.indexOf(';',3)).toInt();
        mRigth = Message.substring(Message.indexOf(';',3)+1,Message.indexOf(';',Message.indexOf(';',3)+1)).toInt();
        if (Message.equals("C;0;0;"))
        {
          Serial.println("Connect");          
          robot_connect();
        } else if (Message.equals("E;0;0;")) {
          Serial.println("Exit");          
          robot_connect();
        } else {
          Serial.print("Go: ");
          Serial.print(mLeft);
          Serial.print(";");
          Serial.print(mRigth);
          Serial.println(";");
          robot_go();
          
        }
          
        time_start = millis();
        Message = "";
        } else { 
           Message = Message + char(incomingByte);  
        }
        //blue.println(char(incomingByte));
      }
}

void robot_connect() 
{ 
  int myTime = 50;
  digitalWrite(led, !digitalRead(led));
  delay(myTime);
  digitalWrite(led, !digitalRead(led));
  delay(myTime);
  digitalWrite(led, !digitalRead(led));
  delay(myTime);  
  digitalWrite(led, !digitalRead(led));
  delay(myTime);
  digitalWrite(led, !digitalRead(led));
  delay(myTime);
  digitalWrite(led, !digitalRead(led));
  delay(myTime);
  digitalWrite(led, !digitalRead(led));
  delay(myTime);
  digitalWrite(led, !digitalRead(led));
  delay(myTime);
  digitalWrite(led, !digitalRead(led));
  delay(myTime);
  digitalWrite(led, !digitalRead(led));
}

void robot_go() 
{ 
  if (mLeft>=0) {
   if (mLeft==0) {
      analogWrite(11, 0);  
      analogWrite(10, 0); 
   } else {
      analogWrite(11, mLeft);  
      analogWrite(10, 0);     
   } 
  } else {
      analogWrite(11, 0);  
      analogWrite(10, mLeft);         
  }
  if (mRigth>=0) {
   if (mRigth==0) {
      analogWrite(5, 0);  
      analogWrite(6, 0); 
   } else {
      analogWrite(5, mRigth);  
      analogWrite(6, 0);     
   } 
  } else {
      analogWrite(5, 0);  
      analogWrite(6, mRigth);         
  }
 
}

 void myTimeout()
 {
  Serial.println("-=Timeout=-");
   mLeft = 0;
   mRigth = 0;
   robot_go();
   time_start = millis();
 }





