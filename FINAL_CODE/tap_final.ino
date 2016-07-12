#include <Wire.h>
#include <math.h>
#include <TimeLib.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

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

#define Circuit_pin 3 // reading from our circuit
#define LOCK 7
#define OPEN 130
double get_angle();
int get_waterFlow();
int time_hour();
int time_min();
int time_sec();

void leak_present();

int time_sec()
{
  int time_s;
   if (timeStatus() == timeSet) {
  time_s = second();
   }
     return time_s;
}
int time_hour()
{
  int time_hr;
  if (timeStatus() == timeSet) {
    time_hr = hour();
  }
  return time_hr;
  
}


int time_min()
{
    int time_m;
    if (timeStatus() == timeSet) {
    time_m =minute();
  }
  return time_m;
  
}


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
//    Serial.print("Flow rate: ");
 //   Serial.print(int(flowRate));  // Print the integer part of the variable
 //   Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac = (flowRate - int(flowRate)) * 10;
 //   Serial.print(frac, DEC) ;      // Print the fractional part of the variable
 //   Serial.print("L/min");
    // Print the number of litres flowed in this second
  //  Serial.print("  Current Liquid Flowing: ");             // Output separator
  //  Serial.print(flowMilliLitres);
 //   Serial.print("mL/Sec");

    // Print the cumulative total of litres flowed since starting
 //   Serial.print("  Output Liquid Quantity: ");             // Output separator
 //   Serial.print(totalMilliLitres);
 //   Serial.println("mL"); 

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

  setSyncProvider(RTC.get);   // the function to get the time from the RTC
 /* if(timeStatus()!= timeSet) 
     Serial.println("Unable to sync with the RTC");
  else
     Serial.println("RTC has set the system time");  
}*/

void leak_present()
{
    int head ;
    int circuitRead;
    int time_m;
    int time_s;
    int do_nothing;
    int offset;
    int flag = 0;
    int flag2 = 0 ;
   int flow_in_millilitres;
     flow_in_millilitres = get_waterFlow();
   
    time_s  = time_sec();
    head = get_angle();
    circuitRead = analogRead(Circuit_pin);
    
    if( (head == LOCK || head == LOCK+1 || head == LOCK-1) && (circuitRead >=400))
    {
       time_s  = time_sec();
       offset =   time_s + 2;
       while (time_sec() < offset)
       {
         if( (head == LOCK || head == LOCK+1 || head == LOCK-1)  && (circuitRead >=400))
         {
           head = get_angle();
           do_nothing = 1;
         }
         else
         {
          flag = 1;
          break;
         }
       }
       if (flag !=1)
       {
          Serial.println("alert detected");

           time_s  = time_sec();
           offset =   time_s+ 4;
           while(time_sec() < offset)
           {
                if ((head == LOCK || head == LOCK+1 || head == LOCK-1)  && (circuitRead >=400))
                {
                   head = get_angle();
                   do_nothing = 1;
                }
                 else
                {
                   flag2 = 1;
                  break;
                 
                 }
           }
           if (flag2 !=1)
           {
            
              Serial.println("alert exeeded");
           }
           
}}


  
     else if((flow_in_millilitres < 50) && (flow_in_millilitres > 0) && (head > OPEN -5) )
    
    {
       time_s  = time_sec();
       offset =   time_s + 2;
       while (time_sec() < offset)
       {
         if((flow_in_millilitres < 50) && (flow_in_millilitres > 0) && (head > OPEN -5))
         {
           do_nothing = 1;
            head = get_angle();
         }
         else
         {
          flag = 1;
          break;
         }
       }
       if (flag !=1)
       {
          Serial.println("alert detected");
            Serial.print("leakage in pipe or tank water shortage");

        time_s  = time_sec();
       offset =   time_s + 5;
           while(time_sec() < offset)
           {
                if ((flow_in_millilitres < 50) && (flow_in_millilitres > 0) && (head > OPEN -4))
                {
                     do_nothing = 1;
                     head = get_angle();
                }
                 else
                {
                   flag2 = 1;
                  break;
                 
                 }
           }
           if (flag2 !=1)
           {
            
              Serial.println("alert exeeded");
           }
           
}}
 
        
       
}

void loop() {

  leak_present();
  
  delay(500);
}

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
