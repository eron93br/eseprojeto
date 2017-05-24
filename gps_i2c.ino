#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>     

// Tamanho do Pacote de Dados
#define PACKET_SIZE 16

// Variavel definida como uniao de caracteres
typedef union float2bytes_t   
{ 
  float f; 
  byte b[sizeof(float)]; 
}; 

const byte addrSlaveI2C =  21;  // I2C Slave address of this device
byte I2C_Packet[PACKET_SIZE];  // Array to hold data sent over I2C to main Arduino
bool printDataflag = false;

// Choose two Arduino pins to use for software serial
// The GPS Shield uses D2 and D3 by default when in DLINE mode
int RXPin = 2;
int TXPin = 3;

// The Skytaq EM-506 GPS module included in the GPS Shield Kit
// uses 4800 baud by default
int GPSBaud = 4800;

// Create a TinyGPS++ object called "gps"
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup()
{
  // Start the Arduino hardware serial port at 9600 baud
  Serial.begin(9600);
  Serial.println(F("Initialize wire library for slave I2C"));
  Wire.begin(addrSlaveI2C);    // Initiate the Wire library and join the I2C bus 
  Wire.onRequest(wireRequestEvent); // Register a function to be called when a master requests data from this slave device. 
  delay(50);
  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);
  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
}

void loop()
{
  rotinai2c();
  // This sketch displays information every time a new sentence is correctly encoded.
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

// Send data to Master.  This is an interrupt driven event
void wireRequestEvent()
{
  // Send byte array from panStamp. Main Arduino will decode bytes
  Wire.write(I2C_Packet, PACKET_SIZE); 
    printDataflag = true;
} 

void rotinai2c()
{
  float2bytes_t f2b_temp; 
  float2bytes_t f2b_temp_bast;
  float2bytes_t f2b_umidade; 
  float2bytes_t f2b_corrente; 
  
  
  f2b_temp.f = (gps.location.lat());
  f2b_temp_bast.f = ((gps.location.lng()));
  f2b_umidade.f=3333.33;
  f2b_corrente.f=4444.44;
  
  
  
  I2C_Packet[0] = f2b_temp.b[0];
  I2C_Packet[1] = f2b_temp.b[1];
  I2C_Packet[2] = f2b_temp.b[2];
  I2C_Packet[3] = f2b_temp.b[3];

  I2C_Packet[4] = f2b_temp_bast.b[0];
  I2C_Packet[5] = f2b_temp_bast.b[1];
  I2C_Packet[6] = f2b_temp_bast.b[2];
  I2C_Packet[7] = f2b_temp_bast.b[3];

  I2C_Packet[8] = f2b_umidade.b[0];
  I2C_Packet[9] = f2b_umidade.b[1];
  I2C_Packet[10] =f2b_umidade.b[2];
  I2C_Packet[11] =f2b_umidade.b[3];

  I2C_Packet[12] = f2b_corrente.b[0];
  I2C_Packet[13] = f2b_corrente.b[1];
  I2C_Packet[14] =f2b_corrente.b[2];
  I2C_Packet[15] =f2b_corrente.b[3];

 if (printDataflag)
  {
    PrintData(f2b_temp.f, f2b_temp_bast.f,f2b_umidade.f,f2b_corrente.f);
    printDataflag = false;
  }  
}

void PrintData( float temp,float tempbast,float umidade,float corrente )
{
  Serial.print("\t");
  Serial.print("Tempreratura: ");
  Serial.print(temp);
  Serial.print("    ");
 
  Serial.print("\t");
  Serial.print("Tempreratura Bastao: ");
  Serial.print(tempbast);
  Serial.print("    ");

  Serial.print("\t");
  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.print("    ");

  Serial.print("\t");
  Serial.print("Corrente: ");
  Serial.print(corrente);
  Serial.print("    ");
  Serial.println();

  delay(500);
}



void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
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
