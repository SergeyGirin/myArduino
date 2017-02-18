#include <Ticker.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <TimeLib.h>
#include "TM1637.h"
#define TM1637_CLK  12
#define TM1637_DIO  14

// Прерывание
Ticker blinker;

TM1637 tm1637(TM1637_CLK, TM1637_DIO);
uint8_t brightness = 2;
bool          points   = true;

String HOST8266 = "http://fh7951dm.bget.ru/8266/";

ESP8266WiFiMulti WiFiMulti;

int myTimeZona = 3*60*60; // +3
time_t myLocalTime = 1474024173 + myTimeZona ; // Store the current time in time variable t 

void displayTm1637()
{
  int8_t my_hour = hour();
  int8_t my_minute = minute();
  points = !points;
  tm1637.point(points);
   
  if (my_hour/10>0) tm1637.display(0,my_hour/10); else tm1637.display(0,0x7f);
  tm1637.display(1,my_hour%10);
  tm1637.display(2,my_minute/10);
  tm1637.display(3,my_minute%10);
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
  if (toSerial == 1)
  {
    Serial.print("[LOG] ");
    Serial.print(result);
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
  IPAddress localIP = WiFi.localIP();
  toLog(1,"IP address: "+String(localIP[0]) + '.' + String(localIP[1]) + '.' + String(localIP[2]) + '.' + String(localIP[3]));
  getTimeHTTP();
}


void setup() {
  delay(1000);
  blinker.attach(1, timerIsr);
  // Инициализация дисплея
  tm1637.init();
  // Установка яркости дисплея
  tm1637.set(brightness);
  setupWiFi();
}

void loop() {
  // put your main code here, to run repeatedly:

}
