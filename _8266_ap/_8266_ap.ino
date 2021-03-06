/*
 * 
 * https://dl.dropboxusercontent.com/u/28031220/esp8266/aESPtemplate.ino
 * 
 */

/* Create a WiFi access point and provide a web server on it. */


#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <Servo.h>
#include <ESP8266HTTPClient.h>
#define DEBUG_ESP_HTTP_CLIENT
#define USE_SERIAL Serial
#include <IRremoteESP8266.h>
#include <Ticker.h>
#include <TimeLib.h>
#include <FS.h>
#include <WebSocketsServer.h>

WebSocketsServer webSocket = WebSocketsServer(8001);

#define DBG_OUTPUT_PORT Serial

HTTPClient http;

// Прерывание

Ticker blinker;

Servo myservo;  // create servo object to control a servo 


int RECV_PIN = 5; //an IR detector/demodulator is connected to D1
int SEND_PIN = 15; //an IR detector/demodulator is connected to D9
IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN); 

/* Set these to your desired credentials. */
const char *ssid = "ESPap";
const int led = 2;
/* const char *password = "thereisnospoon"; */

int servoPos = 0;
int servoStep = 5;


int myTimeZona = 3*60*60; // +3
time_t t = 1463139875 + myTimeZona ; // Store the current time in time variable t 

// ----------------------------------- eeprom
#define CONFIG_WIFI_SSID_DEFAULT "DOM.RU"
#define CONFIG_WIFI_PASSWORD_DEFAULT "9282010085"
#define EEPROM_START 0
uint32_t memcrc; uint8_t *p_memcrc = (uint8_t*)&memcrc;

struct eeprom_data_t {
  char CONFIGssid[15];
  char CONFIGpass[15];
} eeprom_data;
static PROGMEM prog_uint32_t crc_table[16] = {
  0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
  0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

ESP8266WebServer server(80);

File fsUploadFile;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
   for ( uint8_t i = 0; i < server.args(); i++ ) {
    if ( server.argName ( i ) == "param")
    {
      Serial.print(server.arg ( i ));
      Serial.println(";");
    }
  }
  int timeBegin = millis();
  //digitalWrite(led, 0);
  digitalWrite(led, !digitalRead(led));
  myservo.write(90*digitalRead(led));
  //Serial.print(" 1 -");
  //Serial.println(millis()-timeBegin);
  WiFiClient client = server.client();
  //Serial.print("11 -");
  //Serial.println(millis()-timeBegin);
  IPAddress remoteIP = client.remoteIP();
  //Serial.print("12 -");
  //Serial.println(millis()-timeBegin);
  //Serial.print("13 -");
  //Serial.println(millis()-timeBegin);
  String remoteIPstr = String(remoteIP[0]) + '.' + String(remoteIP[1]) + '.' + String(remoteIP[2]) + '.' + String(remoteIP[3]);
  //Serial.print(" 2 -");
  //Serial.println(millis()-timeBegin);
  Serial.println("HTTP connected");
  Serial.println(remoteIPstr);
  //Serial.print(" 3 -");
  //Serial.println(millis()-timeBegin);
  String answer = "<h1>You are connected</h1>\n";
  answer += remoteIPstr + "\n";
  answer += "<br>uptime: ";
  answer += String(millis() / 1000);
  answer += "<br>" + myDate ();
  answer += "<br>URI: ";
  answer += server.uri();
  answer += "<br>\nMethod: ";
  answer += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  answer += "<br>Arguments: ";
  answer += server.args();
  answer += "<br>\n";
  //Serial.print(" 4 -");
  //Serial.println(millis()-timeBegin);
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    answer += " " + server.argName ( i ) + ": " + server.arg ( i ) + "<br>\n";
    analizParam(server.argName ( i ) ,server.arg ( i ));
  }
  //Serial.print(" 5 -");
  //Serial.println(millis()-timeBegin);
  if (server.args()==2)
  {
    writeSettingsESP();
  }
  answer += "<br><form>";
  answer += "ssid: <input type=text name=ssid size=30 maxlength=16 value='";
  answer += eeprom_data.CONFIGssid;
  answer += "' />";
  answer += "<br>password: <input type=text name=password size=30 maxlength=16 value='";
  answer += eeprom_data.CONFIGpass;
  answer += "' />";
  answer += "<br><input type='submit' value='Save'></form>";
  //Serial.print(" 6 -");
  //Serial.println(millis()-timeBegin);
  server.send(200, "text/html", answer);
  //Serial.print(" 7 -");
  //Serial.println(millis()-timeBegin);
  //digitalWrite(led, 1);
}


// ----------------------------------- readSettingsESP -----------------------------------
void readSettingsESP()
{
  Serial.println("-= readSettingsESP() =-");
  int i;
  uint32_t datacrc;
  byte eeprom_data_tmp[sizeof(eeprom_data)];

  EEPROM.begin(sizeof(eeprom_data) + sizeof(memcrc));

  for (i = EEPROM_START; i < sizeof(eeprom_data); i++)
  {
    eeprom_data_tmp[i] = EEPROM.read(i);
  }

  p_memcrc[0] = EEPROM.read(i++);
  p_memcrc[1] = EEPROM.read(i++);
  p_memcrc[2] = EEPROM.read(i++);
  p_memcrc[3] = EEPROM.read(i++);

  datacrc = crc_byte(eeprom_data_tmp, sizeof(eeprom_data_tmp));

  if (memcrc == datacrc)
  {
    memcpy(&eeprom_data, eeprom_data_tmp,  sizeof(eeprom_data));
  }
  else
  {
    
    strncpy(eeprom_data.CONFIGssid, CONFIG_WIFI_SSID_DEFAULT, sizeof(CONFIG_WIFI_SSID_DEFAULT));
    strncpy(eeprom_data.CONFIGpass, CONFIG_WIFI_PASSWORD_DEFAULT, sizeof(CONFIG_WIFI_PASSWORD_DEFAULT));    

  }
  Serial.print("eeprom_data.CONFIGssid = ");
  Serial.print(eeprom_data.CONFIGssid);
  Serial.println(";");
  Serial.print("eeprom_data.CONFIGpass = ");
  Serial.print(eeprom_data.CONFIGpass);
  Serial.println(";");


}
// ----------------------------------- crc_update -----------------------------------
unsigned long crc_update(unsigned long crc, byte data)
{
  byte tbl_idx;
  tbl_idx = crc ^ (data >> (0 * 4));
  crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
  tbl_idx = crc ^ (data >> (1 * 4));
  crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
  return crc;
}
// ----------------------------------- crc_byte -----------------------------------
unsigned long crc_byte(byte *b, int len)
{
  unsigned long crc = ~0L;
  uint8_t i;

  for (i = 0 ; i < len ; i++)
  {
    crc = crc_update(crc, *b++);
  }
  crc = ~crc;
  return crc;
}
// ----------------------------------- writeSettingsESP -----------------------------------
void writeSettingsESP()
{
  Serial.println("-= writeSettingsESP =-");
  Serial.print("eeprom_data.CONFIGssid = ");
  Serial.print(eeprom_data.CONFIGssid);
  Serial.println(";");
  Serial.print("eeprom_data.CONFIGpass = ");
  Serial.print(eeprom_data.CONFIGpass);
  Serial.println(";");
  int i;
  byte eeprom_data_tmp[sizeof(eeprom_data)];

  EEPROM.begin(sizeof(eeprom_data) + sizeof(memcrc));

  memcpy(eeprom_data_tmp, &eeprom_data, sizeof(eeprom_data));

  for (i = EEPROM_START; i < sizeof(eeprom_data); i++)
  {
    EEPROM.write(i, eeprom_data_tmp[i]);
  }
  memcrc = crc_byte(eeprom_data_tmp, sizeof(eeprom_data_tmp));

  EEPROM.write(i++, p_memcrc[0]);
  EEPROM.write(i++, p_memcrc[1]);
  EEPROM.write(i++, p_memcrc[2]);
  EEPROM.write(i++, p_memcrc[3]);


  EEPROM.commit();
}
// -----------------------------------  -----------------------------------
void analizParam(String paramName, String paramArgument)
{
  if (paramName=="ssid")
  {
    paramArgument.toCharArray(eeprom_data.CONFIGssid,15);
  }
    if (paramName=="password")
  {
    paramArgument.toCharArray(eeprom_data.CONFIGpass,15);
  }

}

void WIFIscan() 
{
  Serial.println("-= WIFIscan() =-");
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      if (WiFi.SSID(i) == eeprom_data.CONFIGssid )
      {
        Serial.println("*** WiFi enable ***");
        WiFi.begin ( eeprom_data.CONFIGssid, eeprom_data.CONFIGpass );
      }
      delay(10);
    }
  }
  delay(300);
  Serial.print("WiFi.localIP :");
  Serial.println(WiFi.localIP());
  Serial.print("WiFi.softAPIP :");
  Serial.println(WiFi.softAPIP());
  delay(5000);
  //getHTTP();
}

void  encodingIR (decode_results *results)
{
  //Serial.print("encodingIR->results->decode_type = ");
  //Serial.println(results->decode_type);
  if (results->decode_type != UNKNOWN && results->value != 0xFFFFFFFF) 
  {
    Serial.print("code = ");  
    Serial.println(results->value, HEX);
    if ( results->value == 0xFF02FD )
    {
      digitalWrite(led, !digitalRead(led));
      myservo.write(90*digitalRead(led));
    }
    //Serial.println(results->bits, DEC);
    delay(500);
    irSend(results->value);
    delay(500);
    irSend(results->value);
    
  } 
  if (results->decode_type == UNKNOWN && results->rawlen>30) dumpCode(results);
}
int myCount = 0;
void timerIsr()
{
   setTime(t++);
   digitalWrite(led, !digitalRead(led));
   //digitalWrite(SEND_PIN, !digitalRead(SEND_PIN));
   myservo.write(servoRun ());
   Serial.println(myDate());
   myCount++;
   if (myCount>10)
   {
    myCount = 0;
    
    //irSend(0x20DF22DD);
   }
   
/*   Serial.printf("%02d",day());
   Serial.print(".");
   Serial.printf("%02d",month());
   Serial.print(".");
   Serial.print(year());
   Serial.print(" ");
   Serial.printf("%02d",hour());
   Serial.print(":");
   Serial.printf("%02d",minute());
   Serial.print(":");
   Serial.printf("%02d\n",second());
*/   
}

int servoRun ()
{
  if (servoPos>150 && servoStep>0)  servoStep = -servoStep;
  if (servoPos <= 0 && servoStep<0) servoStep = -servoStep;
  servoPos += servoStep;
  return servoPos;
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

void getHTTP()
{
  
  String host =  "http://fh7951dm.bget.ru/100/timer.php";
  http.begin(host); //HTTP

  Serial.println("[HTTP] GET...");
  Serial.println(host);
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
      t = payload.toInt()+myTimeZona;
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void irSend(int  myCod)
{
  //int  myCod = 0x20DF22DD ;
  Serial.print("Send = ");
  Serial.println(myCod, HEX);
  irsend.sendNEC(myCod, 36);

}

void connectWifi(char* mySSID, char* myPASS)
{
  WiFi.begin(mySSID, myPASS);
  int myBegin = millis();
  int myConnect = 1;
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis()-myBegin>9000) 
    {
      myConnect = 0;
      break;
    }
  }

  Serial.println("");
  if (myConnect)Serial.println("WiFi connected");  
  else Serial.println("WiFi not connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
  

}

void mySinhro()
{
  String curentTime = myDate();
   if (curentTime.substring(14) == "00:00") 
   {
    //getHTTP();
   }
}

void sendIrCond() {

  //unsigned int  rawData[173] = {3100,3800, 2050,1000, 1050,2050, 1950,1050, 1000,1050, 1000,1050, 1000,950, 1000,1050, 1000,2050, 2000,1000, 1000,2050, 2000,1050, 1000,950, 1000,1050, 1000,1050, 1000,1050, 1000,1000, 1000,1050, 1000,1100, 950,1050, 1000,1000, 1000,1050, 950,1100, 1000,1050, 1000,1000, 1000,1050, 1000,1050, 1000,1050, 1000,2000, 2000,1050, 3000,3900, 2000,1050, 1000,2150, 1850,1050, 1000,1050, 1000,1100, 950,1000, 1000,1050, 1000,2100, 2000,1000, 1000,2050, 2000,1050, 1000,950, 1000,1050, 1000,1050, 1000,1100, 1000,1000, 1000,1100, 950,1100, 950,1100, 1000,1000, 1000,1050, 1000,1050, 1000,1050, 1000,1000, 1000,1100, 950,1100, 950,1050, 1000,2000, 2000,1050, 3000,3900, 2000,1050, 1000,2050, 1900,1100, 1000,1100, 1000,1150, 900,1000, 950,1050, 1000,2100, 1950,1000, 1000,2050, 2000,1050, 1000,1000, 1000,1050, 950,1100, 1000,1100, 950,1000, 1000,1100, 950,1100, 1000,1100, 1000,1000, 1000,1050, 1000,1100, 1000,1050, 1000,1000, 950,1100, 50};  // UNKNOWN 1D1E8A7A
  //unsigned int  rawData[181] = {3200,3700, 2050,1000, 1050,2050, 1950,1050, 1000,1050, 1000,1050, 1000,950, 1000,1050, 1000,2050, 2000,1000, 1000,2050, 2000,1050, 1000,950, 1000,1050, 1000,1050, 1000,1100, 1000,1000, 1000,1050, 1000,1050, 1000,1050, 1000,1000, 1000,1100, 950,1050, 1000,1100, 950,1000, 1000,1100, 1000,1100, 950,1050, 1000,2000, 2000,1100, 3000,3950, 2000,1100, 950,2100, 1900,1150, 950,1100, 1000,1100, 1000,950, 1000,1100, 1000,2050, 2000,1000, 950,2050, 2000,1050, 1000,1000, 1000,1100, 950,1100, 1000,1100, 950,1000, 1000,1100, 1000,1100, 1000,1050, 1000,1000, 1000,1100, 1000,1100, 950,1150, 900,1000, 950,1050, 1000,1050, 1000,1100, 1000,2000, 2000,1100, 2950,3950, 2000,1100, 950,2100, 1900,1100, 950,1100, 1000,1050, 950,1000, 1000,1100, 950,2050, 2000,1000, 1000,2100, 1900,1150, 950,1000, 950,1100, 1000,1100, 950,1100, 950,1000, 950,1050, 1000,1100, 1000,1100, 950,1000, 1000,1100, 950,1100, 1000,1100, 1000,1000, 1000,1050, 1000,1100, 950,1100, 950,2000, 2000,1100, 3900};  // UNKNOWN D9466C8D
  unsigned int  rawData[187] = {3050,3850, 2200,900, 1200,1900, 1100,850, 2150,900, 1150,900, 1150,800, 1150,900, 1150,1900, 1150,900, 1050,900, 2150,950, 1150,950, 1100,850, 1050,1000, 1100,1000, 1050,1000, 1050,950, 1050,1050, 1000,1050, 1000,1050, 1050,950, 1000,1050, 1000,1050, 1050,1050, 1100,900, 1050,1050, 1050,1000, 1250,800, 1050,1900, 2050,1050, 3050,3900, 2000,1050, 1000,2050, 950,1050, 2000,1050, 1000,1050, 1000,950, 1000,1050, 1000,2100, 1000,1050, 900,1050, 2000,1050, 1000,1050, 1000,950, 1000,1050, 1000,1050, 1000,1050, 1000,1000, 1000,1050, 1000,1050, 1000,1050, 1000,1000, 1000,1050, 1000,1050, 950,1150, 1000,950, 1000,1050, 1000,1050, 1000,1050, 1000,2000, 2000,1100, 3000,3900, 2000,1050, 1000,2100, 900,1050, 2000,1050, 1000,1050, 1000,1000, 1000,1050, 1000,2100, 1000,1100, 900,1050, 2000,1050, 1000,1100, 950,1000, 1000,1050, 1000,1050, 1000,1050, 1000,1000, 1000,1050, 1000,1050, 1000,1050, 1000,950, 1000,1050, 950,1100, 1000,1050, 1000,1000, 1000,1050, 1000,1050, 1000,1050, 1000,2000, 2000,1050, 3850};  // UNKNOWN 3E9E1893


  irsend.sendRaw(rawData,187,35);
  Serial.println("IR sened");
  /*
  delay(1000);
  unsigned int  rawData2[99] = {3050,3850, 2100,1000, 1150,1950, 1950,1000, 1050,1050, 1050,1050, 1050,950, 1000,1050, 1000,2050, 2000,1000, 1000,2100, 2000,1050, 1000,1000, 1000,1050, 1000,1050, 1000,1050, 1000,950, 1000,1050, 1000,1050, 1000,1100, 950,950, 1000,1050, 1000,1050, 1000,1050, 1000,950, 1000,1050, 1000,1050, 1000,1050, 1000,2000, 2050,1050, 3000,3900, 2000,1050, 1000,2050, 1900,1050, 1000,1050, 1000,1050, 1000,1000, 1000,1050, 1000,2050, 2000,1000, 1000,2050, 2000,1100, 950,950, 1000,1050, 1000,1050, 1000,1050, 1000,950, 1000,1050, 1000,1050, 1000};  // UNKNOWN F970AFC1
  irsend.sendRaw(rawData,99,38);
  Serial.println("IR sened");
  delay(1000);
  unsigned int  rawData3[99] = {3250,3700, 2000,1050, 1050,2000, 1950,1050, 1000,1050, 1000,1050, 1000,950, 1000,1050, 1000,2100, 2000,1000, 1000,2050, 2000,1100, 1000,950, 1000,1050, 1000,1100, 950,1100, 1000,1000, 950,1100, 1000,1100, 1000,1050, 1000,1000, 950,1100, 1000,1100, 1000,1100, 1000,1000, 1000,1050, 1000,1100, 950,1050, 1000,2000, 2000,1100, 3000,3950, 2000,1100, 950,2100, 1900,1100, 1000,1100, 1000,1050, 1000,950, 1000,1100, 1000,2100, 2000,1000, 1000,2100, 2000,1100, 1000,950, 1000,1100, 950,1100, 1000,1100, 1000,1000, 950,1100, 1000,1100, 1000};  // UNKNOWN F970AFC1
  irsend.sendRaw(rawData3,99,38);
  Serial.println("IR sened");
  */
}

void  dumpCode (decode_results *results)
{
  // Start declaration
  Serial.print("unsigned int  ");          // variable type
  Serial.print("rawData[");                // array name
  Serial.print(results->rawlen - 1, DEC);  // array size
  Serial.print("] = {");                   // Start declaration

  // Dump data
  for (int i = 1;  i < results->rawlen;  i++) {
    Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
    if ( i < results->rawlen-1 ) Serial.print(","); // ',' not needed on last one
    if (!(i & 1))  Serial.print(" ");
  }

  // End declaration
  Serial.print("};");  // 

  // Comment
  //Serial.print("  // ");
  //encoding(results);
  //Serial.print(" ");
  //ircode(results);

  // Newline
  Serial.println("");

  // Now dump "known" codes
  if (results->decode_type != UNKNOWN) {

    // Some protocols have an address
    if (results->decode_type == PANASONIC) {
      Serial.print("unsigned int  addr = 0x");
      Serial.print(results->panasonicAddress, HEX);
      Serial.println(";");
    }

    // All protocols have data
    Serial.print("unsigned int  data = 0x");
    Serial.print(results->value, HEX);
    Serial.println(";");
  }
}

//format bytes
String formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

void handleFileList() {
  Serial.println("handleFileList()");
  if(!server.hasArg("dir")) {server.send(500, "text/plain", "BAD ARGS"); return;}
  
  String path = server.arg("dir");
  DBG_OUTPUT_PORT.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while(dir.next()){
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  
  output += "]";
  server.send(200, "text/json", output);
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
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void FileRead() {
  handleFileRead("/home.htm");
}

void handleFileUpload(){
  //if(server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    //String filename = "/home.htm";
    if(!filename.startsWith("/")) filename = "/"+filename;
    DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
      fsUploadFile.close();
    DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void getHome()
{
  String host =  "http://fh7951dm.bget.ru/100/home.html";
  http.begin(host); //HTTP

  Serial.println("[HTTP] GET home.html");
  Serial.println(host);
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      File fsHomeFile;
      fsHomeFile = SPIFFS.open("/home.htm", "w");
      String payload = http.getString();
      fsHomeFile.print(payload);
      fsHomeFile.close();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

            if(payload[0] == '#') {
                // we get RGB data

                // decode rgb data
                uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

                //analogWrite(LED_RED,    ((rgb >> 16) & 0xFF));
                //analogWrite(LED_GREEN,  ((rgb >> 8) & 0xFF));
                //analogWrite(LED_BLUE,   ((rgb >> 0) & 0xFF));
            }

            break;
    }

}


void setup() {
  Serial.setDebugOutput(true);
  delay(1000);
  pinMode(led, OUTPUT);
  //pinMode(SEND_PIN, OUTPUT);
  
 
  myservo.attach(4);  // attaches the servo on D2 (GIO4) to the servo object 
  irrecv.enableIRIn();  // Start the receiver
  irsend.begin();
  
  readSettingsESP();
  Serial.begin(115200);
  Serial.println("BEGIN");
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) { 
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DBG_OUTPUT_PORT.printf("\n");
  }
  
  //connectWifi( eeprom_data.CONFIGssid, eeprom_data.CONFIGpass );
 
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  /* WiFi.softAP(ssid, password); */
  WiFi.softAP(ssid); 
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/list", HTTP_GET, handleFileList);
  server.on("/load", HTTP_GET, handleFileUpload);
  server.on("/read", HTTP_GET, FileRead);
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
  
  connectWifi( eeprom_data.CONFIGssid, eeprom_data.CONFIGpass );
  
  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(led, 1);
  //getHTTP();
  getHome();

  // Прерывание 1 сек.
  blinker.attach(1, timerIsr);

  // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

}

void loop() {
  server.handleClient();
  //if ((millis()/1000)%300 == 0) WIFIscan();
  decode_results  results;        // Somewhere to store the results
  if (irrecv.decode(&results)) {  // Grab an IR code
    encodingIR(&results);
    irrecv.resume();              // Prepare for the next value
  }
  //if (millis()%1000 == 0) mySinhro();
  //if (millis()%3000 == 0) sendIrCond();
}



