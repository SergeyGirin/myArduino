//#include <IRremote.h>
#include <IRremoteESP8266.h>
int RECV_PIN = 5; //an IR detector/demodulator is connected to D1
int SEND_PIN = 15; //an IR sender/demodulator is connected to D8
IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN); //an IR led is connected to GPI15 pin D8

boolean recording = true;
decode_results results;

void setup()
{
  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the receiver
  irsend.begin();
}

void loop() {
  if (recording) {
    if (irrecv.decode(&results)) {
      Serial.println("IR code recorded!");
      //irrecv.resume(); // Receive the next value
      Serial.print("Recorded ");
      Serial.print(results.rawlen);
      Serial.println(" intervals.");
      recording = false;
    }
  } else {
    // replay mode
    //delay(2000);
    Serial.println("Sending recorded IR signal!");
    irsend.sendRaw((unsigned int*) results.rawbuf, results.rawlen, 38);
    delay(2000);
  }
}

