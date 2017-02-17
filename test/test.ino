#include <Ticker.h>
#include <ESP8266WiFiMulti.h>
#include <FS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266httpUpdate.h>
#include <TimeLib.h>

String VER = "4";


//String HOST8266 = "http://xen-unix.stc.donpac.ru/8266/";
String HOST8266 = "http://fh7951dm.bget.ru/8266/";

const int LED_PIN = 2;

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
  }
  
  if(path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  if( SPIFFS.exists(path)){
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
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
  String curentTime = myDate();
   if (curentTime.substring(14) == "00:13") 
   {
    toLog(1,"Time Synchronization");
    getTimeHTTP();
   }
   if (curentTime.substring(11) == "14:00:00") 
   {
    toLog(1,logRotate());
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

void setup() {
  setTime(myLocalTime);
  setupSerial();
  setupFs();
  pinMode(LED_PIN, OUTPUT);
  setupWiFi();
  setupWebServer();
  getTimeHTTP();
  // Прерывание 1 сек.
  blinker.attach(1, timerIsr);
}

void loop() {
  server.handleClient();
  if (millis()%1000 == 0)
  {
    mySinhro(); 
  }
}
