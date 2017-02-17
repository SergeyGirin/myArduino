int pin_dir = 6;
int pin_step = 3;
int pin_led = 13;


unsigned long time_limit_led = 100;
unsigned long time_led;

void setup() {
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_step, OUTPUT);
  pinMode(pin_led, OUTPUT);
  time_led = millis();
  digitalWrite(pin_led, 1);
  digitalWrite(pin_dir, 0);
  digitalWrite(pin_step, 0);

}

void loop() {
  // мигалка светодиодом
  if (time_led + time_limit_led < millis()) {
    digitalWrite(pin_led, !digitalRead(pin_led));
    digitalWrite(pin_step, !digitalRead(pin_step));
    time_led = millis();
  }


}
