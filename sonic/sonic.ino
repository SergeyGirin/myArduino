int SONIC1 = 5;
int SONIC2 = 6;
int SOUND = 7;
int LED = 8;
unsigned long duration;
unsigned long time;
int STORAGE = 0;
void setup() {
  pinMode(SONIC1, OUTPUT);
  pinMode(SONIC2, INPUT);
  pinMode(SOUND, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  time = millis();


}

void loop() {
  digitalWrite(SONIC1, 0);
  delayMicroseconds(2);
  digitalWrite(SONIC1, 1);
  delayMicroseconds(10);
  digitalWrite(SONIC1, 0);
  duration = pulseIn(SONIC2, HIGH) / 29 / 2;
  Serial.println(duration);
  Serial.println(STORAGE);
  if (duration < 30)
  {
    digitalWrite(SOUND, 1);
    digitalWrite(LED, 1);
    STORAGE++;

  } else {
    digitalWrite(SOUND, 0);
    digitalWrite (LED, 0);
  }
  blinker();
  delay(200);
}

void blinker() {
  if ( millis() - time >= 1000)
  {
    if (STORAGE > 4) digitalWrite(LED, !digitalRead(LED));
    time = millis();
    if (STORAGE > 10)STORAGE = 0;
  }

}


