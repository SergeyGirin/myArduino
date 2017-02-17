
#include <SoftwareSerial.h>

int incomingByte = 0;   // переменная для хранения полученного байта
int key_move = 0;
unsigned long key_time = 0;
unsigned long speed_time = 0;
boolean send = false;


int led = 13;
float i = 0;
float b = 50000;
float c = 48;
int a = 0;
uint8_t speed = 255; //максимальная скорость по умолчанию
uint8_t speed_k = 45;
int speed_temp = 3000;
int speed_move = 100;

#define init {D4_Out; D5_Out; D6_Out; D7_Out; D8_Out; D11_Out; D12_Out;} 


SoftwareSerial blue(2,3);
char s;

void setup() {
    Serial.begin(9600); // устанавливаем последовательное соединение
    pinMode(led, OUTPUT);
    blue.begin(9600);
}
 
 
void loop() {
    i = i+1;
    if (Serial.available() > 0) {  //если есть доступные данные
        // считываем байт
        incomingByte = Serial.read();
 
        // отсылаем то, что получили
        Serial.print("I received: ");
        //blue.print("I received: ");
        
        //Serial.println(char(incomingByte));
        //blue.println(char(incomingByte));
    }
    if (blue.available() > 0) 
    {  //если есть доступные данные
        // считываем байт
        incomingByte = blue.read();
 
        // отсылаем то, что получили
        //Serial.print("I received blue: ");
        //blue.print("I received blue: ");
          
        //Serial.println(char(incomingByte));
        //blue.println(char(incomingByte));
        switch (incomingByte)  // Смотрим какая команда пришла 
		{   
			case 'T': // Остановка робота 
				key_move = 0;
				robot_stop(); 
				break;  
 
			case 'W': // Движение вперед 
				key_move = incomingByte;
				key_time = millis();
                robot_go();   
                break;   
         
            case 'D': // Поворотjт влево 
				key_move = incomingByte;
				key_time = millis();
                robot_rotation_left();  
                break; 

            case 'A': // Поворот вправо 
				key_move = incomingByte;
				key_time = millis();
                robot_rotation_right();  
                break;  
         
            case 'S': // Движение назад 
				key_move = incomingByte;
				key_time = millis();
                robot_back();  
                break;  
         
            case '1': // 
                set_speed1();  
                break; 
         
            case '2': // 
                set_speed2();  
                break;  
         
            case '3': // 
                set_speed3();  
                break;  
         
            case '4': // 
                set_speed4();  
                break;  
         
            case '5': // 
                set_speed5();  
                break;                
        }
    }
	if (key_move>0) // Нажата кнопка
	{
		speed_time =  millis() - key_time; // Время нажатия
                
		if (speed_time<=500 && speed_move<=speed)
		{
                       // Serial.println(speed_time);
			speed_temp = speed_temp + 1;
			if (speed_temp % 300 == 0)
			{
				speed_move = speed_temp/30;
                                if (speed_move>255){speed_move=255;}
				send = true;
                                Serial.println(speed_move);
			}
		} 
		
		if (send)
		{
			switch (key_move)  // 
			{   
				case 'T': // 
					robot_stop(); 
					break;
				case 'W': // 
					robot_go();   
					break;            
				case 'D': // 
					robot_rotation_left();  
					break; 
				case 'A': // 
					robot_rotation_right();  
					break;         
				case 'S': // 
					robot_back();  
					break;  
			}
			send = false;
		}
	}
    if (incomingByte>=49 and incomingByte<=54)
    {
            b = (incomingByte-c)*10000;
            incomingByte = 0;
            //Serial.println(b);
            //blue.println(b);
    }
    if (b<10000)
    {
        b = 10000;
    }

    if (i>b) 
	{
            //Serial.println(a);
            //blue.println(a);
        if (a>0)
        {
                digitalWrite(led, HIGH);
                a = 0;
        } else {
                digitalWrite(led, LOW);
                a = 1;    
        }
            i = 0;
    }
}

void robot_go() 
{ 

  analogWrite(5, speed_move-speed_move/3+speed_k);  
  analogWrite(6, 0); 
  analogWrite(10, speed_move-speed_move/3+speed_k); 
  analogWrite(11, 0); 
 
} 

void robot_back() 
{ 
    
   analogWrite(5, 0);  
   analogWrite(6, speed_move-speed_move/3+speed_k); 
   analogWrite(10, 0); 
   analogWrite(11, speed_move-speed_move/3+speed_k); 
} 

void robot_stop() 
{ 
   
  analogWrite(5, 0);  
  analogWrite(6, 0); 
  analogWrite(10, 0);
  analogWrite(11, 0);
  speed_temp = 3000;
  speed_move = 100;
} 

void robot_rotation_left() 
{ 
   
  analogWrite(5, speed_move);  
  analogWrite(6, 0); 
  analogWrite(10, 0);
  analogWrite(11, speed_move-speed_k); 
  
} 

void robot_rotation_right() 
{ 
   
  analogWrite(5, 0);  
  analogWrite(6, speed_move);  
  analogWrite(10, speed_move-speed_k);
  analogWrite(11, 0);
   
}  

void set_speed1() 
{ 
  speed = 150;  
  speed_k = 30;
}

void set_speed2() 
{ 
  speed = 180;  
  speed_k = 45;
}

void set_speed3() 
{ 
  speed = 210; 
 speed_k = 45; 
}

void set_speed4() 
{ 
  speed = 230;  
  speed_k = 45;
  Serial.println("set_speed4");
}

void set_speed5() 
{ 
  speed = 255;  
  speed_k = 45;
  Serial.println("set_speed5");
}

