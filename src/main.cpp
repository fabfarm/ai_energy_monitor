#include <Arduino.h>

// Tested and compiled with no errors
// measuring AC current using ACS712 current sensor with ESP32 Microcontroller
// The ACS712 works with high voltage AC so be careful !
// source - /www.circuitschools.com
 
//#include <LiquidCrystal_I2C.h>
 
const int sensorIn = 34;      // pin where the OUT pin from sensor is connected on Arduino
int mVperAmp = 185;           // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
int Watt = 0;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

 
// ***** function calls ******
float getVPP()
{
  float result;
  int readValue;                // value read from the sensor
  int maxValue = 0;             // store max value here
  int minValue = 4096;          // store min value here ESP32 ADC resolution
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the minimum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 3.3)/4096.0; //ESP32 ADC resolution 4096
      
   return result;
 }
 
// initialize the LCD library with I2C address and LCD size
//LiquidCrystal_I2C lcd (0x27, 16,2); 
 
void setup() {
  Serial.begin (9600); 
  Serial.println ("ACS712 current sensor"); 
  // Initialize the LCD connected 
//lcd. init ();
// Turn on the backlight on LCD. 
// lcd. backlight ();
// lcd.print ("ACS712 current");
// lcd. setCursor (0, 1);
// lcd.print ("sensor");
// delay(1000);
// lcd.clear();  
}
 
void loop() {
  Serial.println (""); 
  Voltage = getVPP();
  VRMS = (Voltage/2.0) *0.707;   //root 2 is 0.707
  AmpsRMS = ((VRMS * 1000)/mVperAmp)-0.3; //0.3 is the error I got for my sensor
 
  Serial.print(AmpsRMS);
  Serial.print(" Amps RMS  ---  ");
  Watt = (AmpsRMS*240/1.2);
  // note: 1.2 is my own empirically established calibration factor
// as the voltage measured at D34 depends on the length of the OUT-to-D34 wire
// 240 is the main AC power voltage – this parameter changes locally
  Serial.print(Watt);
  Serial.println(" Watts");
//   lcd. setCursor (0, 0);
//   lcd.print (AmpsRMS);
// lcd.print (" Amps ");
// //Here cursor is placed on first position (col: 0) of the second line (row: 1) 
// lcd. setCursor (0, 1);
// lcd.print (Watt);
// lcd.print (" watt ");
delay (100);
}
