 
 
// Die LED wird über den Arduino Pin 2 angeschlossen 
int ledPin = 8;
// Der FC-04 wird über den analogen Arduino Pin A0 angeschlossen 
int AudioPin = A0;
 
// Mit dieser Variable wird der Schwellenwert eingestellt
int Schwellenwert = 500;
 
// Variable für den ermittelten Geräuschpegel
int volume;
 
void setup() {                
  //Serial.begin(115200); // Für den debug Modus
  pinMode(ledPin, OUTPUT);     
}
 
void loop() {
  
  // Geraeuschsensor auslesen
  volume = analogRead(AudioPin);
  
    //Debug Modus gibt bei Bedarf das Volumen über die serielle Schnittstelle aus
    //Serial.println(volume);
    //delay(10);
  
  if(volume<=Schwellenwert){
    digitalWrite(ledPin, !digitalRead(ledPin)); // Ist das Geräusch höher als der Schwellenwert, wird die LED eingeschaltet
    delay(200);
  }
    
  //else{
  //  digitalWrite(ledPin, LOW); // Ist das Geräusch niedriger als der Schwellenwert, wird die LED ausgeschaltet
  //}
 
}
