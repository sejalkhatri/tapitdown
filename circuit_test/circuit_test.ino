#define Circuit_pin 1

void setup() {
  Serial.begin(9600);

}

void loop() {
  
   int reading;
   reading = analogRead(Circuit_pin);
  // Serial.println("CIRCUIT READING"); 
   Serial.println(reading); 
   //Serial.print("\n\n"); 
   delay(50);
   
   
   
}
