#include "IRremote.h"
#include <Servo.h> 
#include <SD.h>


File myFile;
 
//вход ик приёмника к А0
const int IR_PIN = A0;
// Переменная 'k' для хранения команды с пульта
int k = 0;
 
//создаём объект ик приёмника
IRrecv irrecv(IR_PIN);

Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
// Переменная 'pos' для хранения текущей позиции сервы 
int pos = 0;    // variable to store the servo position 


////////////////////////////////////////////////////////////////////////////
#include <DS1302.h>
//   http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
const int kCePin   = 5;  // Chip Enable
const int kIoPin   = 6;  // Input/Output
const int kSclkPin = 7;  // Serial Clock

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}
Time t_old = rtc.time();
////////////////////////////////////////////////////////////////////////////

// Миллисекунды
unsigned long mtime = 0;
unsigned period = 55000;



void setup (){
  Serial.begin(115200);
  Serial.println("ready");
  //начинаем прослушивание ик сигналов
  irrecv.enableIRIn();
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  myservo.write(pos);
  /////////////////////////////////////////////////// 
  rtc.writeProtect(false);
  rtc.halt(false);

  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.

  // Set the time and date on the chip.
  //Time t(2015, 1, 16, 14, 01, 00, Time::kThursday);
  //rtc.time(t);
  ///////////////////////////////////////////////////

 
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  // pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test2.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test2.txt...");
    myFile.println("testing 1, 2, 3.");
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test2.txt");
  }
  
  // re-open the file for reading:
  myFile = SD.open("test2.txt");
  if (myFile) {
    Serial.println("test.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
    	Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening test2.txt");
  } 
}
 
void loop() {
 //в results будут помещаться принятые и 
 //декодированные ик команды
 decode_results results;
 //Если ик команда принята и успешно декодирована - 
 //выводим полученный код в сириал монитор
 if (irrecv.decode(&results)) {
  Serial.print("IR results.value = "); 
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
    case 0xD55F5981:
      k = 21;
      break;    
    case 0x1FE40BF:
      k = 21;
      break;    
    case 0x8EEF4B83:
      k = 22;
      break;
    case 0x20DFC03F:
      k = 22;
      break;
    case 0x1FE58A7:
      k = 22;
      break;   
    case 0xE7D0D303:
      k = 7;
      break;
    case 0x20DFE817:
      k = 7;
      break;
      
      
  }
  Serial.print("K = ");
  Serial.println(k);
  switch (k) {
    case 22:
      run(pos+10);
    break;
    case 21:
    run(pos-10); 
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
  Serial.print("POS = ");
  Serial.println(pos);
  k = 0;  
  irrecv.resume();
  
 }
// Работаем с микросекундами

// Микросекунды закончились

  
// Работаем с часовым механизмом

  Time t = rtc.time();
  if (t.sec==t_old.sec) {
  } else {
    t_old=t;
    const String day = dayAsString(t.day);
    // Format the time and date and insert into the temporary buffer.
    char buf[50];
    snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);
     // Print the formatted string to serial so we can see the time.
  //   Serial.println(buf);
  // Каждые 5 секунд
    if (t.sec % 5==0) {
      if (millis()-mtime>=period){
        run(pos+10);       
        Serial.print("mtime = ");
        Serial.println(mtime);
        Serial.println(buf);
      }
      

  //    run(t.sec*2); 
   //   Serial.println(k);
    //  k = 0;
    }
    if (t.sec==20) {
      // run(0); 
    }

  }
// Часы закончились

  //Serial.println(Time);
  //delay (50);
  //k += 1;
  // Конец общего цикла
}

// процедура медленного поворота
void run(int n_pos)
{
  //Serial.println("RUN");
  //Serial.println(n_pos);
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
  Serial.print("run POS = ");
  Serial.println(pos);
  // Делаю отсечку времени
  mtime = millis();
  Serial.print("run mtime = ");
  Serial.print(mtime);
  Serial.print("     ");
  Serial.println(period);
  

  
}

