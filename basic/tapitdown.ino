#include <Wire.h>
#include <math.h>

byte statusLed    = 13;
byte FMsensorInterrupt = 0;  // 0 = digital pin 2
byte FMsensorPin       = 2;
// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;
volatile byte pulseCount;  
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

#define Addr 0x1E   // 7-bit address of HMC5883 compass

double get_angle();
int get_waterFlow();

int get_waterFlow()
{
  if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(FMsensorInterrupt);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

    // Print the cumulative total of litres flowed since starting
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres);
    Serial.println("mL"); 

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(FMsensorInterrupt, pulseCounter, FALLING);
  }
  return flowMilliLitres;
}

double get_angle()
{
  int x, y, z;
  double heading;

  // Initiate communications with hmc583l
  Wire.beginTransmission(Addr);
  Wire.write(byte(0x03));       // Send request to X MSB register
  Wire.endTransmission();

  Wire.requestFrom(Addr, 6);    // Request 6 bytes; 2 bytes per axis
  if(Wire.available() <=6)      // If 6 bytes available
  {    
    x = Wire.read() << 8 | Wire.read();
    z = Wire.read() << 8 | Wire.read();
    y = Wire.read() << 8 | Wire.read();
  }
  
  // Print raw values
  Serial.print("X=");
  Serial.print(x);
  Serial.print(", Y=");
  Serial.print(y);
  Serial.print(", Z=");
  Serial.println(z);

  heading = atan2(y,x);
  if(heading<0)
  {
    heading += 2*M_PI;
  }
  heading =heading*180/M_PI;
  return heading;
 
}

void setup() {
  Serial.begin(9600);
  delay(100);                   
  Wire.begin();
  
  Wire.beginTransmission(Addr); 
  Wire.write(byte(0x02));       //Mode register address
  Wire.write(byte(0x00));       // Set operating mode to continuous
  Wire.endTransmission();

     
  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
  pinMode(FMsensorPin, INPUT);
  digitalWrite(FMsensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  attachInterrupt(FMsensorInterrupt, pulseCounter, FALLING);
}



void loop() {
  double heading;
  int flow_in_millilitres;
  heading = get_angle();
  Serial.print("Heading: ");
  Serial.print(heading);
   Serial.print("\n");
  delay(500);
  flow_in_millilitres = get_waterFlow();
  Serial.print("flow_in_millilitres: ");
  Serial.print(flow_in_millilitres);
  Serial.print("\n");
  
}

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
