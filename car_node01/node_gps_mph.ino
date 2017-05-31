/*
 *-------------------------------------------------------------
    Projeto ESE - módulo node_gps_mph
    Recebe informação do GPS, codifica e manda em I2C....
    TAMANHO DO PACOTE I2C:    09 bytes 
  *-------------------------------------------------------------
*/
#include <Wire.h>    
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define PACKET_SIZE 9

//=========The GPS Shield pinout=============
int RXPin = 2;
int TXPin = 3;
int GPSBaud = 4800;
//===========================================

typedef union float2bytes_t   
{ 
  double f; 
  byte b[sizeof(double)]; 
}; 

// I2C 
const byte addrSlaveI2C =  21;  // I2C Slave address of this device
byte I2C_Packet[PACKET_SIZE];  // Array to hold data sent over I2C to main Arduino
bool printDataflag = false;

// Create a TinyGPS++ object called "gps"
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup()
{
  // Start the Arduino hardware serial port at 9600 baud
  Serial.begin(9600);
  delay(50);
  Wire.begin(addrSlaveI2C);    // 
  
  Wire.onRequest(wireRequestEvent); 
  
  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);

  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
  pinMode(13, OUTPUT);
}

int velocidade;

void loop()
{
  float2bytes_t var1; 
  float2bytes_t var2;
  float2bytes_t var3;
  float2bytes_t var4;

  var1.f = 1000000 * gps.location.lat(); 
  var2.f = 1000000 * gps.location.lng(); 
  velocidade = (int)gps.speed.mph();
  
  I2C_Packet[0] = var1.b[0];
  I2C_Packet[1] = var1.b[1];
  I2C_Packet[2] = var1.b[2];
  I2C_Packet[3] = var1.b[3];
  I2C_Packet[4] = var2.b[0];
  I2C_Packet[5] = var2.b[1];
  I2C_Packet[6] = var2.b[2];
  I2C_Packet[7] = var2.b[3];
  I2C_Packet[8] = velocidade;
  
  // get GPS data!
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected"));
    while(true);
  }
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    digitalWrite(13,LOW);
  }
  else
  {
    Serial.print(F("INVALID"));
          digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(5);                       // wait for a second
          digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
       delay(5);
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void wireRequestEvent()
{
  Wire.write(I2C_Packet, PACKET_SIZE); 
  printDataflag = true;
}

