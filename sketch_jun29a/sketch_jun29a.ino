  

unsigned long old_time = 0;
unsigned long new_time = 0;
int x = 1;
int x_old = 1;
int val = 0;
int val_old = 0;
int i = 0;
int kod = 0;


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
      if ( val>4500 && val<4600 && val_old>9000 && val_old<9100  )
    {
      kod = 1;
      Serial.println();
      Serial.print( "(9000,");
    }
    //Serial.print( new_time);         // записываем данные в порт
    //Serial.print( ";");         // записываем данные в порт
    //Serial.print( x);         // записываем данные в порт
    //Serial.print( ";");         // записываем данные в порт
    if (kod = 1) {
      Serial.print( val );
      Serial.print( "," );
    }
    //Serial.println( ";");         // записываем данные в порт
    x_old = x;
    if ( val>540 && val<570 && val_old>2200 && val_old<2290 )
    {
      kod = 0;
      Serial.println(")");
      Serial.println( "===========");
      Serial.println( i);
      
      i = 0;
    }
    val_old = val;
    old_time = new_time ;
    if (kod = 1) i++;
    
    }

  

}


