#include <Ticker.h>
#include <ESP8266WiFiMulti.h>
#include <FS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266httpUpdate.h>
#include <TimeLib.h>
#include <IRremoteESP8266.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "TM1637.h"



Servo myservo;  // create servo object to control a servo 
const int SERVO_PIN = 4;//attaches the servo on D2 (GPIO4) to the servo object 

String VER = "12";
const int LED_PIN = 2;
const int ZUMMER_PIN = 13; //an pikalka is connected to D7 (GPIO13)
const int RELE_PIN = 12; //an pikalka is connected to D6 (GPIO12)
const int IRRECV_PIN = 5; //an IR detector/demodulator is connected to D1 (GPIO5)
const int ONE_WIRE_BUS = 14; //an pikalka is connected to D5 (GPIO14)

int servoPosBase = 180; // базовое состояние сервопривода
int servoPos = servoPosBase; // текущее состояние сервопривода
long servoTimeLimit = long(10) * long(60) * long(1000);
long servoTimeStart = 0;

IRrecv irrecv(IRRECV_PIN);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer = { 0x28, 0xFF, 0xC1, 0x71, 0x54, 0x14, 0x00, 0xB2 };

TM1637 tm1637(0, 15);



//String HOST8266 = "http://xen-unix.stc.donpac.ru/8266/";
String HOST8266 = "http://fh7951dm.bget.ru/8266/";

ESP8266WiFiMulti WiFiMulti;

// Прерывание
Ticker blinker;

ESP8266WebServer server(80);

int myTimeZona = 3*60*60; // +3
time_t myLocalTime = 1474024173 + myTimeZona ; // Store the current time in time variable t 


void timerIsr()
{
  setTime(myLocalTime++);
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  displayTm1637();
}

void displayTm1637()
{
  tm1637.point(!digitalRead(LED_PIN));
  tm1637.display(0,hour()/10);
  tm1637.display(1,hour()%10);
  tm1637.display(2,minute()/10);
  tm1637.display(3,minute()%10);
}

String handleFileList() {
  String path = "/";
  Dir dir = SPIFFS.openDir(path);
  path = String();
  String output = "";
  while(dir.next())
  {
    File entry = dir.openFile("r");
    output += String(entry.name()).substring(1);
    output += ";";
    entry.close();
  }
  return output;
}

String ls() {
  String path = "/";
  Dir dir = SPIFFS.openDir(path);
  path = String();
  String output = "";
  while(dir.next())
  {
    File entry = dir.openFile("r");
    output += String(entry.name()).substring(1);
    output += "\t";
    if (String(entry.name()).substring(1).length()<15)
    {
      output += "\t";
    }
    output += String(entry.size());
    output += "\n";
    entry.close();
  }
  output += getFreeFs();
  output += "\n";
  return output;
}

String getFreeFs()
{
      FSInfo info;
      SPIFFS.info(info);
      int freeBytes = info.totalBytes - info.usedBytes;
      String output = "";
      output += "totalBytes = ";
      output += String(info.totalBytes);
      output += " ; usedBytes = ";
      output += String(info.usedBytes);
      output += " ; freeBytes = ";
      output += String(freeBytes);
      return output;
}


void loadFile(String fileName)
{
  toLog(1,"loadFile("+fileName+")");
  String host =  HOST8266+String(ESP.getChipId())+"/"+fileName;
  HTTPClient http;
  http.begin(host); //HTTP
  toLog(1,host);
  // start connection and send HTTP header
  int httpCode = http.GET();
  if(httpCode > 0) 
  {
    toLog(1,"[HTTP] GET... code: "+String(httpCode));
    if(httpCode == HTTP_CODE_OK) 
    {
      int len = http.getSize();
      toLog(1,getFreeFs());
      if(SPIFFS.exists("/"+fileName))
      {
        File inspectFile;
        inspectFile = SPIFFS.open("/"+fileName, "r");
        if (inspectFile.size() == http.getSize())
        {
          toLog(1,"Files ["+fileName+"] equal size.");
          inspectFile.close();
          return;
        }
      }
      // create buffer for read
      //uint8_t buff[128] = { 0 };
      uint8_t buff[1500] = { 0 };
      File fsLoadFile;
      fsLoadFile = SPIFFS.open("/"+fileName, "w");
      if (len == -1)
      {
        fsLoadFile.print(http.getString());
      }
      else 
      {
        // get tcp stream
        WiFiClient * stream = http.getStreamPtr();
        // read all data from server
        int lenAll = len;
        float  percent = 0;
        float  percentOld = 0;
        
        while(http.connected() && (len > 0 || len == -1)) 
        {
            // get available data size
            size_t size = stream->available();  
            if(size) 
            {
                // read up to buffer
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                // write it to 
                fsLoadFile.write(buff, c);
                delay(1);
                if(len > 0) 
                {
                    len -= c;
                }
                percent = (float(lenAll) - float(len))/float(lenAll)*float(100);              
                if (int(percent) > (int(percentOld) + int(4)) or  int(percent)==100)
                {
                  toLog(1,"percent = "+String(percent)+"% ; len ="+String(len)+" ; size ="+String(fsLoadFile.size())+" ; c ="+String(c));              
                  percentOld = percent;
                  fsLoadFile.flush();
                }
                digitalWrite(LED_PIN, !digitalRead(LED_PIN));
            }
            delay(1);
        }
      }

      toLog(1,"[HTTP] connection closed or file end.");
      fsLoadFile.flush();
      fsLoadFile.close();
      toLog(1,handleFileList());
    } else {
      //Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      toLog(1,"[HTTP] GET... failed, error: "+String(http.errorToString(httpCode).c_str()));
    }
  }
  http.end();
  digitalWrite(LED_PIN, 0);
}

void handleRoot() {
  WiFiClient client = server.client();
  handleFileRead(server.uri());
}

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  toLog(1,"handleFileRead("+path+")");
  int isContinue = 1;
  WiFiClient client = server.client();
  IPAddress remoteIP = client.remoteIP();
  String remoteIPstr = String(remoteIP[0]) + '.' + String(remoteIP[1]) + '.' + String(remoteIP[2]) + '.' + String(remoteIP[3]);
  toLog(1,"HTTP connected from "+remoteIPstr+" url:"+server.uri());
  for ( uint8_t i = 0; i < server.args(); i++ ) 
  {
    String argName = server.argName(i);
    String arg = server.arg(i);
    argName.toLowerCase();
    arg.toLowerCase();
    server.arg ( i ).toLowerCase();
    toLog(1,"[arg] "+argName+" = "+arg);
    if ( argName == "messages" )
    {
      isContinue = 0;
      if (arg =="clear")
      {
          server.send(200, "text/plain", logRotate());
      }
      if (arg == "size")
      {
          logSize();
      } 
    }
    if ( argName == "file" )
    {
      isContinue = 0;
      if (arg == "list")
      {
        server.send(200, "text/plain", handleFileList());
      }
      if (arg == "ls")
      {
        server.send(200, "text/plain", ls());
      }
    }
    if ( argName == "deletefile" )
    {
      isContinue = 0;
      if (SPIFFS.exists(arg))
      {
        if (SPIFFS.remove(arg))
        {
          server.send(200, "text/plain", "File "+arg+" deleting.");
        } 
        else
        {
          server.send(200, "text/plain", "File "+arg+" not deleting.");
        }
      }
    }
    if (argName=="param")
    {
      isContinue = 0;
      if (arg=="2" or arg=="3" or arg=="4" or arg=="5" or arg=="6" or arg=="7" )
      {
        boiler(arg.toInt());
        server.send(200, "text/html", String(arg) + ";1;");
      } 
    }
    if (argName=="base")
    {
      isContinue = 0;
      if (arg=="2" or arg=="3" or arg=="4" or arg=="5" or arg=="6" or arg=="7" )
      {
        if (arg=="7") {servoPosBase = 0;    servoRun(0);}
        if (arg=="6") {servoPosBase = 30;   servoRun(30);}
        if (arg=="5") {servoPosBase = 60;   servoRun(60);}
        if (arg=="4") {servoPosBase = 100;  servoRun(100);}
        if (arg=="3") {servoPosBase = 150;  servoRun(150);}
        if (arg=="2") {servoPosBase = 180;  servoRun(180);}
        toBase(String(servoPosBase));

        boiler(arg.toInt());
        server.send(200, "text/html", "Base param setting.");
      } 
      if (arg=="0") {server.send(200, "text/html",  String(servoPosBase) + ';');}
    }
    if (argName == "servo" and arg == "1")
    {
      isContinue = 0;
      server.send(200, "text/html", String(servoPos) + ";1;");
    }
    if (argName == "time")
    {
      isContinue = 0;
      getTimeHTTP();
      server.send(200, "text/html", "Time Synchronization.");
    }
    if (argName == "restart")
    {
      isContinue = 0;
      server.send(200, "text/html", "Restart.");
      ESP.restart();
    }
    if (argName == "reload")
    {
      isContinue = 0;
      loadWebFiles();
      server.send(200, "text/html", "Files reloaded.");
    }
    if (argName == "termo" and arg == "0")
    {
      isContinue = 0;
      server.send(200, "text/html", String(getTemperature(insideThermometer)) + ";");
    }

    
  }

  if (isContinue == 1)
  {
    if (path.endsWith("/")) path += "index.html";
    String contentType = getContentType(path);
    if( SPIFFS.exists(path)){
      File file = SPIFFS.open(path, "r");
      size_t sent = server.streamFile(file, contentType);
      file.close();
      return true;
    }
  }
  return false;
}


void split(char * buf)
{
  char * tmpbuf = strtok(buf, ";");
  while(tmpbuf != NULL) 
  {
    Serial.println((char *)tmpbuf);
    tmpbuf = strtok(NULL, ";");
  }
}

void loadWebFiles()
{
  toLog(1,"loadWebFiles()");
  String host =  HOST8266+"list_file.php?chipid="+String(ESP.getChipId());
  toLog(1,host);
  HTTPClient http;
  http.begin(host); //HTTP
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    toLog(1,"[HTTP] GET... code: "+String(httpCode));

    // file found at server
    if(httpCode == HTTP_CODE_OK) 
    {
      String payload = http.getString();
      toLog(1,payload);
      // split
      int bufLength = payload.length()+1;
      char buf[bufLength];
      payload.toCharArray(buf,bufLength);            
      char * tmpbuf = strtok(buf, ";");
      while(tmpbuf != NULL) 
      {
        loadFile(String(tmpbuf));
        tmpbuf = strtok(NULL, ";");
      }
    }
  } else {
    toLog(1,"[HTTP] GET... failed, error: %s\n"+String(http.errorToString(httpCode).c_str()));
  }
  http.end(); 
}

class Splitter
{
  public:
  String str;
  String splitter;
  int parts;
  int begin(String source, String split);
  String part(int p);
};

int Splitter::begin(String source, String split) {
  splitter = split;
  str = source;
  String y = source;
  int count = -1;
  while (y.indexOf(splitter)>0)
  {
    count++;
    int p = y.indexOf(splitter);   
    String x = y.substring(0,p);
    y = y.substring(p+1);
  }
  parts = count;
  return count;
}

String Splitter::part(int part) {
  String y = str;
  int count = -1;
  while (y.indexOf(splitter)>0)
  {
    count++;
    int p = y.indexOf(splitter);   
    String result = y.substring(0,p);
    y = y.substring(p+1);    
    if (count == part)
    {
      count = -1;
      return result;
    } 
  }
}

String getListFiles()
{
  toLog(1,"getListFiles()");
  String host =  HOST8266+"list_file.php?chipid="+String(ESP.getChipId());
  String payload = "-1";
  HTTPClient http;
  http.begin(host); 
  int httpCode = http.GET();
  if(httpCode > 0) {
    toLog(1,"[HTTP] GET... code: "+String(httpCode));
    if(httpCode == HTTP_CODE_OK) 
    {
      payload = http.getString();
      toLog(1,payload);
    }
  } else {
    toLog(1,"[HTTP] GET... failed, error: "+String(http.errorToString(httpCode).c_str()));
  }
  http.end(); 
  return payload;
}

void fileListCompare()
{
  toLog(1,"fileListCompare(begin)");
  String remote = getListFiles();
  if (remote.compareTo("-1")==0)
  {
    return;
  }
  Splitter www;
  www.begin(remote,";");
  Splitter local;
  local.begin(handleFileList(),";");

  for (int i=0;i<=local.parts;i++)
  {
    String fileName = local.part(i);
    toLog(1,fileName);
    int exist = -1;
    for (int j=0;j<=www.parts;j++)
    {
      if (fileName.compareTo(www.part(j))==0)
      {
        exist = 1;
      }
    }
    //if (exist<0 and !fileName.compareTo("log/messages")==0)
    if (exist<0 and fileName.indexOf("log/messages")<0)
    {
      toLog(1,fileName + " !!! file not exists, remove file "+fileName);
      SPIFFS.remove("/"+fileName);
    }
  }
  toLog(1,handleFileList());
}

String getFileForUpdate()
{
  toLog(1,"getFileForUpdate()");
  // http://fh7951dm.bget.ru/8266/list_file.php?chipid=8800703&version=1
  String host =  HOST8266+"list_file.php?chipid="+String(ESP.getChipId())+"&version="+VER;
  String payload = "-1";
  HTTPClient http;
  http.begin(host);
  toLog(1,host);
  int httpCode = http.GET();
  if(httpCode > 0) {
    toLog(1,"[HTTP] GET... code: " + String(httpCode));
    if(httpCode == HTTP_CODE_OK) 
    {
      payload = http.getString();
      toLog(1,payload);
    }
  } else {
    toLog(1,"[HTTP] GET... failed, error: " + String(http.errorToString(httpCode).c_str()));
  }
  http.end(); 
  return payload;
}

void updateESP()
{
  toLog(1,"updateESP()");
  String fileLoad = getFileForUpdate();
  if (fileLoad.compareTo("-1")==0)
  {
    toLog(1,"HTTP_UPDATE_NO_UPDATES for version "+VER);
    return;
  }
  t_httpUpdate_return ret = ESPhttpUpdate.update(HOST8266+String(ESP.getChipId())+"/bin/"+fileLoad);
  //t_httpUpdate_return ret = ESPhttpUpdate.update("https://server/file.bin");

  switch(ret) 
  {
      case HTTP_UPDATE_FAILED:
          toLog(1,"HTTP_UPDATE_FAILD Error ("+String(ESPhttpUpdate.getLastError())+"): "+String(ESPhttpUpdate.getLastErrorString().c_str()));
          break;

      case HTTP_UPDATE_NO_UPDATES:
          toLog(1,"HTTP_UPDATE_NO_UPDATES");
          break;

      case HTTP_UPDATE_OK:
          toLog(1,"HTTP_UPDATE_OK");
          break;
  }
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

void mySinhro()
{
   String currentTime = myDate();
   if (currentTime.substring(14) == "00:13" or currentTime.substring(14) == "30:13") 
   {
    toLog(1,"Time Synchronization");
    getTimeHTTP();
   }
   if (currentTime.substring(11) == "00:00:00") 
   {
    toLog(1,logRotate());
   }
   // контроль возвращения сервопривода на базу
   if (servoPosBase!=servoPos)
   {
      if (second()%3 == 0)
      {
        zummer();
      }
      if (long(servoTimeStart) + long(servoTimeLimit) < millis())
      {
        servoRun(servoPosBase);
      }
   }
   //if (currentTime.substring(15) == "3:16" or currentTime.substring(15) == "8:16")
   if (currentTime.substring(15) == "3:16")
   {
    float currentTemperature = getTemperature(insideThermometer);
    String result = "Temp C: "+ String(currentTemperature);
    //toLog(1,result);
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

void toLog(String str)
{
  
  File logFile = SPIFFS.open("/log/messages", "a+");
  logFile.print(str);
  logFile.close();
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
  toLog(result);
  if (toSerial == 1)
  {
    Serial.print("[LOG] ");
    Serial.print(result);
  }  
}

String logRotate()
{
  toLog(1,"logRotate(begin)");
  if (SPIFFS.exists("/log/messages.7")) SPIFFS.remove("/log/messages.7");
  if (SPIFFS.exists("/log/messages.6")) SPIFFS.rename("/log/messages.6", "/log/messages.7");
  if (SPIFFS.exists("/log/messages.5")) SPIFFS.rename("/log/messages.5", "/log/messages.6");
  if (SPIFFS.exists("/log/messages.4")) SPIFFS.rename("/log/messages.4", "/log/messages.5");
  if (SPIFFS.exists("/log/messages.3")) SPIFFS.rename("/log/messages.3", "/log/messages.4");
  if (SPIFFS.exists("/log/messages.2")) SPIFFS.rename("/log/messages.2", "/log/messages.3");
  if (SPIFFS.exists("/log/messages.1")) SPIFFS.rename("/log/messages.1", "/log/messages.2");
  if (SPIFFS.exists("/log/messages.0")) SPIFFS.rename("/log/messages.0", "/log/messages.1");
  if (SPIFFS.exists("/log/messages")) SPIFFS.rename("/log/messages", "/log/messages.0");
  toLog(1,"logRotate(end)");
  return "/log/messages rotating.";
}

void logSize()
{
  File logFile = SPIFFS.open("/log/messages", "r");
  toLog(1,"/log/messages size="+String(logFile.size()));
  server.send(200, "text/plain", String(logFile.size()));
  logFile.close();
}

void toBase(String str)
{  
  File logFile = SPIFFS.open("/base.txt", "w");
  logFile.println(str);
  logFile.close();
}

void zummer()
{
  digitalWrite(ZUMMER_PIN, 1);
  delay(100);
  digitalWrite(ZUMMER_PIN, 0);
}

void zummerServo()
{
  digitalWrite(ZUMMER_PIN, 1);
  delay(30);
  digitalWrite(ZUMMER_PIN, 0);
}

void zummerBegin()
{
   for ( int i = 0; i < 3; i++ ) 
   {
      digitalWrite(ZUMMER_PIN, 1);
      delay(100);
      digitalWrite(ZUMMER_PIN, 0);
      delay(100);
   }
}

int readKey (unsigned long value)
{
  int result = -1;
    // коды кнопок
    switch (value) {
      case 0x2ADE08A3:
        result = 1;
        break;
      case 0x20DF8877:
        result = 1;
        break;
      case 0x20DF48B7:
        result = 2;
        break;
      case 0x7F613A3B:
        result = 2;
        break;
      case 0x42C7C05B:
        result = 2;
        break;
      case 0xA85740BF:
        result = 2;
        break;        
      case 0x20DF40BF:
        result = 21;
        break;
      case 0x9E0A10FF:
        result = 21;
        break;
      case 0xC3DAC5B2:
        result = 21;
        break;
      case 0x8EEF4B83:
        result = 22;
        break;
      case 0x20DFC03F:
        result = 22;
        break;
      case 0xE7D0D303:
        result = 7;
        break;
      case 0x20DFE817:
        result = 7;
        break;
      case 0xD130B09F:
        result = 0;
        break;
      case 0x1A46B33F:
        result = 3;
        break;
      case 0x20DFC837:
        result = 3;
        break;
      case 0x9F3E1063:
        result = 4;
        break;
      case 0x2FB2625F:
        result = 5;
        break;
      case 0x4CEB59FF:
        result = 6;
        break;
      case 0x20DF6897:
        result = 6;
        break;
      case 0xC984587B:
        result = 8;
        break;
      case 0x2331B07F:
        result = 9;
        break;
      case 0xFFFFFFFF:
        break;
      // samsung
      case 0xF2B862C4:
        result = 1;
        break;
      case 0xFC8C27A4:
        result = 2;
        break;
      case 0xA857C03F:
        result = 3;
        break;
      case 0x34FB828:
        result = 3;
        break;        
      case 0xA85720DF:
        result = 4;
        break;
      case 0xE90FD844:
        result = 4;
        break;   
      case 0xA857A05F:
        result = 5;
        break;
      case 0x589B8648:
        result = 5;
        break;      
      case 0xA857609F:
        result = 6;
        break;
      case 0x2F0207E0:
        result = 6;
        break;       
      case 0xA857E01F:
        result = 7;
        break;
      case 0x35C59864:
        result = 7;
        break;     
      case 0xFD6B90A4:
        result = 8;
        break;
      case 0xA857906F:
        result = 9;
        break;
      case 0x4C65BAC8:
        result = 0;
        break;
      case 0xA857827D:
        result = 22;
        break;
      case 0xA85742BD:
        result = 21;
        break;
      case 0xA857D22D:
        result = 22;
        break;
      case 0xA85732CD:
        result = 21;
        break;
        
      default:
        result = -1;
    }
  return result;
}

void servoRun(int servoRunPos)   // процедура медленного поворота
{
  toLog(1,"servoRun("+String(servoRunPos)+")");
  // включаем индикацию и релюху
  digitalWrite(LED_PIN, 1);
  digitalWrite(RELE_PIN, 0);
  delay(30); // пауза на включение реле
  if (servoRunPos > servoPos) {
    for (int i = servoPos; i <= servoRunPos; i++) {
      servoPos = i;
      myservo.write(servoPos);
      if (i%3 == 0)
      {
        delay(30);
      }
      else
      {
        zummerServo();
      }     
    }
  } 
  else 
  {
    for (int i = servoPos; i >= servoRunPos; i--) 
    {
      servoPos = i;
      myservo.write(servoPos);
      if (i%3 == 0)
      {
        delay(30);
      }
      else
      {
        zummerServo();
      }
    }
  }
  if (servoPos > 180) servoPos = 180;
  if (servoPos < 0) servoPos = 0;
  
  // устанавливаю время старта для таймайта
  servoTimeStart = millis();
  // тушим индикацию и релюху
  digitalWrite(LED_PIN, 0);
  digitalWrite(RELE_PIN, 1);
}


void boiler(int myKey)
{
  switch (myKey) {
        case 22:
          servoRun(servoPos + 10);
          break;
        case 21:
          servoRun(servoPos - 10);
          break;
        case 9:
          break;
        case 8:
          break;
        case 7:
          servoRun(0);
          break;
        case 6:
          servoRun(30);
          break;
        case 5:
          servoRun(60);
          break;
        case 4:
          servoRun(100);
          break;
        case 3:
          servoRun(150);
          break;
        case 2:
          servoRun(180);
          break;
        case 1:
          break;
        case 0:
          break;
      }
}



void  encodingIR (decode_results *results)
{
  if (results->decode_type != UNKNOWN && results->value != 0xFFFFFFFF) 
  {
    zummer();
    String result = String(results->value, HEX);
    result.toUpperCase();
    toLog(1,"[IR] 0x" + result);
    int myKey = readKey(results->value);
    if (myKey>0)
    {
      boiler(myKey);
    }    
  }
}

/////////////////////////////////////////////////////////
void setupFs()
{
  SPIFFS.begin();
  toLog(1,"============================= START =============================");
/*
  Serial.println("format(begin)");
  SPIFFS.format();
  Serial.println("format(end)");
*/  
}

void setupWebServer()
{
  toLog(1,"setupWebServer()");
  server.on("/", handleRoot);
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
  server.begin();
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
  updateESP();
  loadWebFiles();
  fileListCompare();
  toLog(1,getFreeFs());
}

void setupSerial()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
}

void readBase()
{
  if(SPIFFS.exists("/base.txt"))
  {
    toLog(1,"Read /base.txt");
    File fsBaseFile;
    fsBaseFile = SPIFFS.open("/base.txt", "r");
    String base=fsBaseFile.readStringUntil('\n');
    servoPosBase = base.toInt();
    toLog(1,"servoPosBase in file: *"+base+"*");
  }
}

float getTemperature(DeviceAddress deviceAddress)
{
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(deviceAddress);
  toLog(1,"Temp C: "+ String(tempC));
  return tempC;
}

void setupPins()
{
  pinMode(RELE_PIN, OUTPUT);
  myservo.attach(SERVO_PIN); 
  digitalWrite(RELE_PIN, 0);
  delay(30); 
  readBase();
  servoRun(servoPosBase);
  delay(1000); 
  digitalWrite(RELE_PIN, 1);
  servoTimeStart = millis();  
  irrecv.enableIRIn();  // Start the receiver
  sensors.begin();
  sensors.setResolution(insideThermometer, 12);
  // Инициализация дисплея
  tm1637.init();
// Установка яркости дисплея  
  tm1637.set(2);
}

void setup() {
  pinMode(ZUMMER_PIN, OUTPUT);
  zummer();
  setTime(myLocalTime);
  setupSerial();
  setupFs();
  pinMode(LED_PIN, OUTPUT);
  setupWiFi();
  setupWebServer();
  getTimeHTTP();
  // Прерывание 1 сек.
  blinker.attach(1, timerIsr);
  setupPins();
  zummerBegin();
}

void loop() {
  server.handleClient();
  
  decode_results  results;        // Somewhere to store the results
  if (irrecv.decode(&results)) 
  {  // Grab an IR code
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    encodingIR(&results);
    irrecv.resume();              // Prepare for the next value
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
  if (millis()%1000 == 0)
  {
    mySinhro(); 
  }
}
