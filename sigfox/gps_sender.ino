/*
 *-------------------------------------------------------------
    Projeto ESE - módulo node_gps_mph
    Recebe informação do GPS, codifica e manda em I2C....
    TAMANHO DO PACOTE I2C:    09 bytes 
  *-------------------------------------------------------------
*/
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <WISOL.h>
//#include <Tsensors.h>
#include <Wire.h>
#include <math.h>
#include <SimpleTimer.h>

#define PACKET_SIZE 9
byte sigfoxPacket[PACKET_SIZE];

//===============SigFox configs==============
Isigfox *Isigfox = new WISOL();
//Tsensors *tSensors = new Tsensors();
SimpleTimer timer;
//===========================================

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

typedef union
{
    float number;
    uint8_t bytes[4];
} FLOATUNION_t;

typedef union
{
    uint16_t number;
    uint8_t bytes[2];
} UINT16_t;

typedef union
{
    int16_t number;
    uint8_t bytes[2];
} INT16_t;

// Create a TinyGPS++ object called "gps"
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup()
{
  //=========================================== Start GPS setup ===========================================
  Serial.begin(9600);
  delay(50);  
  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);
  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); 
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
  pinMode(13,OUTPUT); 
  // End of GPS setup ======================================================================================

 //=========================================== Start SigFox setup ==========================================
  Wire.begin();
  Wire.setClock(100000);
  // WISOL test
  Isigfox->initSigfox();
  Isigfox->testComms();
  GetDeviceID();
  //Isigfox->setPublicKey(); // set public key for usage with SNEK

  // Init sensors on Thinxtra Module
  /*tSensors->initSensors();
  tSensors->setReed(reedIR);
  tSensors->setButton(buttonIR);
  */
  
  // Init timer to send a SIgfox message every 10 minutes
  unsigned long sendInterval = 600000;
  timer.setInterval(sendInterval, timeIR);
  // End of SigFox setup ====================================================================================
}

int velocidade;

void loop()
{
     timer.run();
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

// SigFox functions

void Send_Sensors()
{
  UINT16_t tempt, photo, pressure;
  INT16_t x_g, y_g, z_g;
  char sendMsg[20];
  int sendlength;
  char sendstr[2];
  //acceleration_xyz *xyz_g;
  FLOATUNION_t a_g;

  /*
  // Sending a float requires at least 4 bytes
  // In this demo, the measure values (temperature, pressure, sensor) are scaled to ranged from 0-65535.
  // Thus they can be stored in 2 bytes
  tempt.number = (uint16_t) (tSensors->getTemp() * 100);
  Serial.print("Temp: "); Serial.println((float)tempt.number/100);
  pressure.number =(uint16_t) (tSensors->getPressure()/3);
  Serial.print("Pressure: "); Serial.println((float)pressure.number*3);
  photo.number = (uint16_t) (tSensors->getPhoto() * 1000);
  Serial.print("Photo: "); Serial.println((float)photo.number/1000);

  xyz_g = (acceleration_xyz *)malloc(sizeof(acceleration_xyz));
  tSensors->getAccXYZ(xyz_g);
  x_g.number = (int16_t) (xyz_g->x_g * 250);
  y_g.number = (int16_t) (xyz_g->y_g * 250);
  z_g.number = (int16_t) (xyz_g->z_g * 250);
  Serial.print("Acc X: "); Serial.println((float)x_g.number/250);
  Serial.print("Acc Y: "); Serial.println((float)y_g.number/250);
  Serial.print("Acc Z: "); Serial.println((float)z_g.number/250);
  free(xyz_g);
  */

    float2bytes_t var1; 
  float2bytes_t var2;
  float2bytes_t var3;
  float2bytes_t var4;

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
  
  var1.f = 1000000 * gps.location.lat();       // LATITUDE!
  var2.f = 1000000 * gps.location.lng();       // LONGITUDE!
  velocidade = (int)gps.speed.mph();
  
  // codificação para latitude
  sigfoxPacket[0] = var1.b[0];
  sigfoxPacket[1] = var1.b[1];
  sigfoxPacket[2] = var1.b[2];
  sigfoxPacket[3] = var1.b[3];
  // codificação para longitude
  sigfoxPacket[4] = var2.b[0];
  sigfoxPacket[5] = var2.b[1];
  sigfoxPacket[6] = var2.b[2];
  sigfoxPacket[7] = var2.b[3];
  sigfoxPacket[8] = velocidade;
  
  int payloadSize = 12; //in byte
//  byte* buf_str = (byte*) malloc (payloadSize);
  uint8_t buf_str[payloadSize];

  buf_str[0] = sigfoxPacket[0];
  buf_str[1] = sigfoxPacket[1];
  buf_str[2] = sigfoxPacket[2];
  buf_str[3] = sigfoxPacket[3];
  buf_str[4] = sigfoxPacket[4];
  buf_str[5] = sigfoxPacket[5];
  buf_str[6] = sigfoxPacket[6];
  buf_str[7] = sigfoxPacket[7];
  buf_str[8] = sigfoxPacket[8];
  buf_str[9] =  0xFF;
  buf_str[10] = 0xAA;
  buf_str[11] = 0x55;

  Send_Pload(buf_str, payloadSize);
//  free(buf_str);
}

void reedIR()
{
  Serial.println("Reed");
  timer.setTimeout(20, Send_Sensors); // send a Sigfox message after get ou IRS
}

void buttonIR()
{
  Serial.println("Button");
  timer.setTimeout(20, Send_Sensors); // send a Sigfox message after get ou IRS
}

void timeIR()
{
  Serial.println("Time");
  Send_Sensors();
}

void getDLMsg()
{
  recvMsg *RecvMsg;
  int result;

  RecvMsg = (recvMsg *)malloc(sizeof(recvMsg));
  result = Isigfox->getdownlinkMsg(RecvMsg);
  for (int i=0; i<RecvMsg->len; i++){
    Serial.print(RecvMsg->inData[i]);
  }
  Serial.println("");
  free(RecvMsg);
}


void Send_Pload(uint8_t *sendData, int len)
{
  // No downlink message require
  recvMsg *RecvMsg;
  Isigfox->resetMacroChannel(); // required to send on first macro channel
  delay(100);

  RecvMsg = (recvMsg *)malloc(sizeof(recvMsg));
  Isigfox->sendPayload(sendData, len, 0, RecvMsg);
  for (int i=0; i<RecvMsg->len; i++){
    Serial.print(RecvMsg->inData[i]);
  }
  Serial.println("");
  free(RecvMsg);


  // If want to get blocking downlink message, use the folling block instead
  /*
  recvMsg *RecvMsg;

  RecvMsg = (recvMsg *)malloc(sizeof(recvMsg));
  Isigfox->sendPayload(sendData, len, 1, RecvMsg);
  for (int i=0; i<RecvMsg->len; i++){
    Serial.print(RecvMsg->inData[i]);
  }
  Serial.println("");
  free(RecvMsg);
  */

  // If want to get non-blocking downlink message, use the folling block instead
  /*
  Isigfox->sendPayload(sendData, len, 1);
  timer.setTimeout(46000, getDLMsg);
  */
}

void GetDeviceID()
{
  int headerLen = 6;
  recvMsg *RecvMsg;
  char msg[] = "AT$I=10";
  int msgLen = 7;

  RecvMsg = (recvMsg *)malloc(sizeof(recvMsg));
  Isigfox->sendMessage(msg, msgLen, RecvMsg);

  Serial.print("Device ID: ");
  for (int i=0; i<RecvMsg->len; i++){
    Serial.print(RecvMsg->inData[i]);
  }
  Serial.println("");
  free(RecvMsg);
}


