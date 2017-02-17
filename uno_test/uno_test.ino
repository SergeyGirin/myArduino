const int LED_PIN = 13;
const int PWM_IN_PIN = 7;
long TIME = millis();
long TIME_OLD = millis();
//#include <Servo.h>


int STBY = 4; //standby

//Motor A
//int PWMA = 9; //Speed control 
int AIN1 = 5; //Direction
int AIN2 = 6; //Direction
int PWMA = 9; //Speed control 

//Servo myservo;  // create servo object to control a servo

int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(PWM_IN_PIN, INPUT);
  Serial.begin(115200);
 // myservo.attach(9);  // attaches the servo on pin 9 to the servo object
 // myservo.write(100); 
  pinMode(PWMA, OUTPUT);  
  pinMode(STBY, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  digitalWrite(STBY, 1); //disable standby
  digitalWrite(AIN1, 1);
  digitalWrite(AIN2, 0);
}

void loop() {
   TIME = millis();
  if (TIME%1000==0 and TIME!=TIME_OLD)
  {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    Serial.print(TIME/1000);
    TIME_OLD = TIME;
    int gradus = random(250);
//    myservo.write(gradus);  
    gradus = 100;
    digitalWrite(AIN1, !digitalRead(AIN1));
    digitalWrite(AIN2, !digitalRead(AIN2));
    int puls = pulseIn(PWM_IN_PIN, HIGH); 
    Serial.print("\tPWM (");
    Serial.print(gradus);
    Serial.print(") = ");
    Serial.print(puls);
    Serial.print(" (");
    Serial.print(map(puls,525,2375,0,180));
    Serial.print(")");
    Serial.print(" ");
    Serial.println(analogRead(0));
    analogWrite(PWMA, gradus);
  }
}
