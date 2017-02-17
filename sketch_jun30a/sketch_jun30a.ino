  

unsigned long old_time = 0;
unsigned long new_time = 0;
int x = 1;
int x_old = 1;
int val = 0;
int val_old = 0;
int i = 0;
int kod = 0;
int start = 0;
int stp = 0;


void setup() {
  Serial.begin(115200); // указываем скорость работы с COM-портом
  //attachInterrupt(5, blink, CHANGE);
  pinMode(5, INPUT);
  /*
  ETS_GPIO_INTR_DISABLE();
  ETS_GPIO_INTR_ATTACH(gpio_intr, NULL);
  gpio_pin_intr_state_set(GPIO_ID_PIN(5), GPIO_PIN_INTR_ANYEDGE);
  ETS_GPIO_INTR_ENABLE();
  */

}

void loop() {
  
  
 
    x = digitalRead(5); 
    if (x != x_old)
    {
      new_time = micros();
      val = new_time - old_time;
      if ( val>3000 && val<4000 && val_old>3000 && val_old<4000  && kod==0) 
      
     
      
    {
      
      kod = 1;
      Serial.println();
      Serial.print("{");
      Serial.print(val_old);
      Serial.print(",");
      //Serial.print(val);
      //Serial.print(",");
    }
    
    
    if (kod == 1) {
      Serial.print( val );
      Serial.print( "," );
      i++;
    }
    else {
    //Serial.print( new_time);         // записываем данные в порт
    //Serial.print( ";");         // записываем данные в порт
    Serial.print( val);         // записываем данные в порт
    Serial.print( ",");         // записываем данные в порт
    Serial.println( x);
    }
    
    
    if ( val>800 && val<900 && val_old>3000 && val_old<4000 )
    {
      stp = 1;
    }
    if (stp == 1 && val>100000 )
    {
      kod = 0;
      Serial.println("};");
      Serial.println( "===========");
      i++;
      i++;
      Serial.println( i);
      
      i = 0;
      stp = 0;
    }
    val_old = val;
    old_time = new_time ;
    x_old = x;
    
    
    }

  

}


