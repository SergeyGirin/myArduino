int LED1 = 3;
int LED2 = 4;
int LED3 = 5;
int SOUND = 6;
void setup() {
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(SOUND, OUTPUT);
}


void loop() {
  digitalWrite(LED1, 0);
  digitalWrite(LED2, 0);
  digitalWrite(LED3, 1);
  digitalWrite(SOUND, 0);
  delay(3000);

  digitalWrite(LED1, 0);
  digitalWrite(LED2, 1);
  digitalWrite(LED3, 1);
  digitalWrite(SOUND, 0);
  delay(1500);

  digitalWrite(LED1, 1);
  digitalWrite(LED2, 0);
  digitalWrite(LED3, 0 );
  digitalWrite(SOUND, 1);
  delay(700);
  digitalWrite(SOUND, 0);
  delay(200);
  digitalWrite(SOUND, 0);
  delay(100);
  digitalWrite(SOUND, 1);
  delay(700);
  digitalWrite(SOUND, 0);
  delay(300);
  digitalWrite(SOUND, 1);
  delay(700);
  digitalWrite(SOUND, 0);
  delay(100);
  digitalWrite(SOUND, 0);
  delay(200);
  digitalWrite(SOUND, 1);
  delay(700);
  digitalWrite(SOUND, 0);
  delay(300);
  digitalWrite(SOUND, 1);
  delay(700);
  digitalWrite(SOUND, 0);
  delay(200);
  digitalWrite(SOUND, 0);
  delay(100);
  digitalWrite(SOUND, 1);
  digitalWrite(LED1, 1);
  delay(700);
  digitalWrite(SOUND, 0);
  digitalWrite(LED1, 0);
  delay(300);
  digitalWrite(SOUND, 1);
  digitalWrite(LED1, 1);
  delay(700);
  digitalWrite(SOUND, 0);
  digitalWrite(LED1, 0);
  delay(100);
  digitalWrite(SOUND, 0);
  digitalWrite(LED1, 0);
  delay(200);
  digitalWrite(SOUND, 1);
  digitalWrite(LED1, 1);
  delay(700);
  digitalWrite(SOUND, 0);
  digitalWrite(LED1, 0);
  delay(300);
  digitalWrite(SOUND, 1);
  digitalWrite(LED1, 1);
  delay(700);
  digitalWrite(SOUND, 0);
  digitalWrite(LED1, 0);
  delay(300);
  digitalWrite(SOUND, 1);
  digitalWrite(LED1, 1);
  delay(300);

  digitalWrite(LED1, 1);
  digitalWrite(LED2, 1);
  digitalWrite(LED3, 0);
  digitalWrite(SOUND, 0);
  delay(1500);
  digitalWrite(LED1, 0);
  digitalWrite(LED2, 0);
  digitalWrite(LED3, 1);
  digitalWrite(SOUND, 0);
  delay(1500);







}
