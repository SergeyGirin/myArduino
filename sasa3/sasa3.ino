int LED = 3;
int LED2 = LED_BUILTIN;
int val = 0;
bool y = 0;
void setup() {
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
}


void loop() {
  
  analogWrite(LED, val);
  if (y)
  {
    val--;
  } else
  {
    val++;
  }

  delay(10);
  if (val >= 255 or val <= 0) y = !y ;




}
