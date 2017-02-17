#include <Ticker.h>
#include "TM1637.h"
#define TM1637_CLK  12
#define TM1637_DIO  14
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <TimeLib.h>

// Прерывание
Ticker blinker;

TM1637 tm1637(TM1637_CLK, TM1637_DIO);
uint8_t brightness = 2;
bool          points   = true;
//uint8_t counter = 0;
int counter = 0;

ESP8266WiFiMulti WiFiMulti;
IPAddress localIP;

String HOST8266 = "http://fh7951dm.bget.ru/8266/";


int myTimeZona = 3*60*60; // +3
time_t myLocalTime = 1474024173 + myTimeZona ; // Store the current time in time variable t 


void displayTm1637()
{
  /*
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
  */
  points = !points;
  tm1637.point(points);
  tm1637.display(0,hour()/10);
  tm1637.display(1,hour()%10);
  tm1637.display(2,minute()/10);
  tm1637.display(3, minute()%10);
}

void timerIsr()
{
  setTime(myLocalTime++);
  displayTm1637();
}

void getTimeHTTP()
{
  toLog(1, "getTimeHTTP()");
  //http://fh7951dm.bget.ru/8266/timer.php?chipid=8800703
  String host =  HOST8266+"timer.php?chipid="+String(ESP.getChipId());
  String payload = "-1";
  HTTPClient http;
  http.begin(host);
  toLog(1, host);
  int httpCode = http.GET();
  if(httpCode > 0) {
    toLog(1, "[HTTP] GET... code: "+String(httpCode));
    // file found at server
    if(httpCode == HTTP_CODE_OK) 
    {
      payload = http.getString();
      toLog(1, payload);
    }
  } else {
    toLog(1, "[HTTP] GET... failed, error: "+String(http.errorToString(httpCode).c_str()));
  }
  http.end(); 
  if (payload.toInt()>0)
  {
    myLocalTime = payload.toInt()+myTimeZona;  
    setTime(myLocalTime);
    toLog(1,myDate());
  }
}

void setupWiFi()
{
  toLog(1,"setupWiFi()");
  WiFi.mode(WIFI_STA);
  WiFi.printDiag(Serial);
  WiFiMulti.addAP("DOM.RU", "9282010085");
  WiFiMulti.addAP("girin", "9282010085");
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  localIP = WiFi.localIP();
  getTimeHTTP();
  
}

void toLog(int toSerial, String str)
{
  String result = "";
  result += myDate();
  result += " ";
  result += str;
  if (str.substring(-1).compareTo("\n")!=0)
  {
    result += "\n";
  }
  //toLog(result);
  if (toSerial == 1)
  {
    Serial.print("[LOG] ");
    Serial.print(result);
  }  
}

String myDate ()
{
  String dateSpliter = ".";
  String timeSpliter = ":";
  char myDay[4], myMonth[4], myHour[4], myMinute[4], mySecond[4];
  sprintf(myDay,"%02d",day());
  sprintf(myMonth,"%02d",month());
  sprintf(myHour,"%02d",hour());
  sprintf(myMinute,"%02d",minute());
  sprintf(mySecond,"%02d",second());
  //String(weekday())
  return String(myDay) + dateSpliter + String(myMonth) + dateSpliter + String(year()) + " " 
          + String(myHour) + timeSpliter + String(myMinute) + timeSpliter + String(mySecond);
}

void setup() {
  delay(1000);
  blinker.attach(1, timerIsr);
  // Инициализация дисплея
  tm1637.init();
  // Установка яркости дисплея
  tm1637.set(brightness);
  setupWiFi();
  getTimeHTTP();
}

void loop() {
  // put your main code here, to run repeatedly:

}
