#include <Wire.h>
#include <math.h>

#define Addr 0x1E   // 7-bit address of HMC5883 compass

double get_angle();
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

void setup() 
{
  Serial.begin(9600);
  delay(100);                   
  Wire.begin();
  
  Wire.beginTransmission(Addr); 
  Wire.write(byte(0x02));       //Mode register address
  Wire.write(byte(0x00));       // Set operating mode to continuous
  Wire.endTransmission();
}

void loop() 
{
  double heading;
  int threshold_value;
  heading = get_angle();
  Serial.print("Heading: ");
  Serial.print(heading);

   delay(500);
}
