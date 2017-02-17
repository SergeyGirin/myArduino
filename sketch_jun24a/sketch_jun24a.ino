#define ANALOGA_IN 0
int i = 0;
int k = 0;
unsigned long old_time = 0;
unsigned long new_time = 0;

void setup() {
  Serial.begin(115200); // указываем скорость работы с COM-портом
  attachInterrupt(0, blink, CHANGE);
  pinMode(2, INPUT_PULLUP);
}

void loop() {
  /*
  int val;
  int i = 0;
  while(1)
  {
    val = analogRead(ANALOGA_IN); // считываем данные 
    Serial.print( micros());         // записываем данные в порт
    Serial.print( ";");         // записываем данные в порт
    Serial.print( val);         // записываем данные в порт
    Serial.print( ";");         // записываем данные в порт
    Serial.print( i);         // записываем данные в порт    
    Serial.println( ";");         // записываем данные в порт
    i ++;
    if (i>10000) i = 0 ;
  }
  */

}


void blink()
{
  k ++;
  i ++;
  
  new_time = micros();
  Serial.print( new_time);         
  Serial.print(";");
  Serial.print(digitalRead(2));
  Serial.print( ";");        
  Serial.print( i);
  Serial.print( ";");        
  Serial.print( new_time - old_time );
  Serial.print( ";");        
  Serial.print( k );

  Serial.println(";");
  old_time = new_time ;
  
  if (i>10000) i = 0 ;
  k = 0;
}
