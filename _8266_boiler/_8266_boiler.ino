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
#include <IRremoteESP8266.h>
#include <Ticker.h>
#include <TimeLib.h>

HTTPClient http;

// Прерывание

Ticker blinker;

Servo myservo;  // create servo object to control a servo 
const int SERVO_PIN = 4;//attaches the servo on D2 (GIO4) to the servo object 


const int IRRECV_PIN = 5; //an IR detector/demodulator is connected to D1 (GIO5)
const int IRSEND_PIN = 15; //an IR detector/demodulator is connected to D8 (GI15)

IRrecv irrecv(IRRECV_PIN);
IRsend irsend(IRSEND_PIN); 

/* Set these to your desired credentials. */
const char *ssid = "ESPap";
const int LED_PIN = 2;
const int ZUMMER_PIN = 13; //an pikalka is connected to D7 (GI13)
const int RELE_PIN = 12; //an pikalka is connected to D6 (GI12)
/* const char *password = "thereisnospoon"; */


int servoPosBase = 180; // базовое состояние сервопривода
int servoPos = servoPosBase; // текущее состояние сервопривода
long servoTimeLimit = long(10) * long(60) * long(1000);
long servoTimeStart = 0;


int myTimeZona = 3*60*60; // +3
time_t myLocalTime = 1463139875 + myTimeZona ; // Store the current time in time variable t 

// ----------------------------------- eeprom
#define CONFIG_WIFI_SSID_DEFAULT "girin"
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

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  int timeBegin = millis();
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  zummer();
  WiFiClient client = server.client(); 
  IPAddress remoteIP = client.remoteIP();
  String remoteIPstr = String(remoteIP[0]) + '.' + String(remoteIP[1]) + '.' + String(remoteIP[2]) + '.' + String(remoteIP[3]);
  Serial.println("HTTP connected");
  Serial.println(remoteIPstr);
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
  answer = "";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    answer += server.arg ( i ) + ";1;";
    analizParam(server.argName ( i ) ,server.arg ( i ));
  }
  if (server.args()==2)
  {
    //writeSettingsESP();
  } 
  if (server.args()==1 and server.argName ( 0 )=="servo")
  {
    answer = String(servoPos) + ";1;";
  }
  server.send(200, "text/html", answer);
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
    if (paramName=="param")
  {
    if (paramArgument=="01")
    {
      sendIrTV();
    }
    if (paramArgument=="02")
    {
      sendIrCond();
    }
    if (paramArgument=="03")
    {
      sendIrSony();
    }
    if (paramArgument=="2" or paramArgument=="3" or paramArgument=="4" or paramArgument=="5" or paramArgument=="6" or paramArgument=="7" )
    {
      boiler(paramArgument.toInt());
    }
    
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
  getTimeHTTP();
}

void  encodingIR (decode_results *results)
{
  if (results->decode_type != UNKNOWN && results->value != 0xFFFFFFFF) 
  {
    zummer();
    //Serial.print("code = ");  
    //Serial.println(results->value, HEX);
    int myKey = readKey(results->value);
    if (myKey>0)
    {
      boiler(myKey);
    }    
  } 
  //if (results->decode_type == UNKNOWN && results->rawlen>30) dumpCode(results);
  dumpCode(results);
}

void timerIsr()
{
   setTime(myLocalTime++);
   Serial.println(myDate());

   // контроль возвращения сервопривода на базу
   if (servoPosBase!=servoPos)
   {
      zummer();
      if (long(servoTimeStart) + long(servoTimeLimit) < millis())
      {
        servoRun(servoPosBase);
      }
   }
}


void servoRun(int servoRunPos)   // процедура медленного поворота
{
  // включаем индикацию и релюху
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(RELE_PIN, LOW);
  delay(30); // пауза на включение реле
  if (servoRunPos > servoPos) {
    for (int i = servoPos; i <= servoRunPos; i++) {
      servoPos = i;
      myservo.write(servoPos);
      delay(30);
    }
  } else {
    for (int i = servoPos; i >= servoRunPos; i--) {
      servoPos = i;
      myservo.write(servoPos);
      delay(30);
    }
  }
  if (servoPos > 180) servoPos = 180;
  if (servoPos < 0) servoPos = 0;
  
  // устанавливаю время старта для таймайта
  servoTimeStart = millis();
  // тушим индикацию и релюху
  digitalWrite(LED_PIN, LOW);
  digitalWrite(RELE_PIN, HIGH);
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

void getTimeHTTP()
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
      myLocalTime = payload.toInt()+myTimeZona;
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
  irsend.sendNEC(myCod, 32);

}

void connectWifi(char* mySSID, char* myPASS)
{
  WiFi.begin(mySSID, myPASS);
  int myBegin = millis();
  int myConnect = 1;
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis()-myBegin>3000) 
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
    getTimeHTTP();
   }
}

void sendIrCond() {
  irrecv.disableIRIn();
  unsigned int  rawData_cond[271] = {3613,3381,1000,740,1008,740,1001,740,1002,2495,995,2496,998,2493,1099,2414,963,2529,962,768,972,792,954,797,949,2540,945,2546,951,2528,957,2552,934,2558,951,797,919,2564,951,2546,928,815,926,827,931,821,914,832,914,832,884,862,873,2616,887,2611,887,855,891,856,878,868,879,873,855,891,3477,3502,881,868,878,867,873,885,873,2611,870,2622,875,2617,881,2611,886,2605,881,912,822,891,850,885,891,2573,854,2669,864,2628,870,2622,863,2628,881,879,850,2646,857,2617,858,890,868,896,856,873,856,891,867,879,856,902,850,2628,864,2635,858,884,868,879,867,879,868,890,850,885,3477,3508,881,13958,3481,3502,875,873,880,873,867,2622,882,2622,863,2617,863,2868,637,2617,887,2604,912,839,873,873,873,2617,881,2611,886,2605,881,2611,887,2610,881,2605,870,879,879,2616,876,2616,870,873,886,2611,887,2604,882,867,885,861,879,862,891,2604,876,2616,893,856,879,2611,881,2611,881,868,873,873,3487,3497,881,867,880,867,879,2611,881,2610,887,2605,887,2605,887,2605,887,2607,884,867,874,867,879,2613,890,2605,881,2610,881,2611,881,2617,875,2617,881,867,867,2623,875,2617,881,861,874,2623,869,2623,875,873,868,879,879,867,873,2617,875,2617,875,873,868,2622,875,2617,874,873,879,867,3489,3495,882};
  irsend.sendRaw(rawData_cond,271,36);
  Serial.println("IR sened Cond");
  irrecv.enableIRIn();
}

void sendIrTV() {
  irrecv.disableIRIn();
  //unsigned int  rawData[71] = {9000,4500,550,600,550,600,550,1700,550,600,550,600,550,600,550,600,550,600,550,1700,550,1700,550,600,550,1700,550,1700,550,1700,550,1700,550,1700,550,600,550,600,550,1700,550,600,550,600,550,600,550,1700,550,600,550,1700,550,1703,550,600,550,1700,550,1700,550,1700,550,600,550,1700,550,40093,9037,2269,550};
  //irsend.sendRaw(rawData,71,35);
  irsend.sendNEC(0x20DF10EF, 32);
  delay(500);
  Serial.println("IR sened TV");
  irrecv.enableIRIn(); 
}

void sendIrSony() {
  irrecv.disableIRIn();
  unsigned int  rawData[25] = {2700,350, 1500,350, 900,350, 1500,350, 900,350, 1500,350, 900,350, 900,400, 850,400, 850,400, 850,450, 850,450, 1400};
  for (int i = 1;  i < 7;  i++) {
    irsend.sendRaw(rawData,25,35);
    delay(50);
  }
  Serial.println("IR sened Sony");
  irrecv.enableIRIn(); 
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

void zummer()
{
  digitalWrite(ZUMMER_PIN, 1);
  delay(100);
  digitalWrite(ZUMMER_PIN, 0);
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

void setup() {
  delay(1000);
  Serial.begin(115200);
   
  pinMode(LED_PIN, OUTPUT);
  pinMode(ZUMMER_PIN, OUTPUT);
  pinMode(RELE_PIN, OUTPUT);
 
  myservo.attach(SERVO_PIN);  
  myservo.write(servoPosBase);
  servoTimeStart = millis();
  
  irrecv.enableIRIn();  // Start the receiver
  irsend.begin();
  
  readSettingsESP();
 
  Serial.println();
 
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid); 
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(LED_PIN, 1);
  
  connectWifi( eeprom_data.CONFIGssid, eeprom_data.CONFIGpass );
  // Прерывание 1 сек.
  blinker.attach(1, timerIsr);
  getTimeHTTP();
}

void loop() {
  server.handleClient();
  decode_results  results;        // Somewhere to store the results
  if (irrecv.decode(&results)) {  // Grab an IR code
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    encodingIR(&results);
    irrecv.resume();              // Prepare for the next value
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
  
  if (millis()%1000 == 0)
  {
    mySinhro(); 
  }

  //if (millis()%600000 == 0) sendIrCond();
  //if (millis()%5000 == 0) sendIrTV();
  
}




