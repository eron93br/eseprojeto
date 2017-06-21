
const int analogPin = A0;

void setup() {
  
Serial.begin(9600);
pinMode(analogPin, INPUT); 
int sensor =0 ;
  
}


void loop()
{
 int16_t  sensor = analogRead(analogPin) ;
   byte vetor[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10};
   Serial.write(vetor,10);
   delay (1000);
}

/*
#include <Wire.h>     

// Tamanho do Pacote de Dados
#define PACKET_SIZE 10

// Variavel definida como uniao de caracteres
typedef union float2bytes_t   
{ 
  float f; 
  byte b[sizeof(float)]; 
}; 


const byte addrSlaveI2C =  21;  // I2C Slave address of this device
byte I2C_Packet[PACKET_SIZE];  // Array to hold data sent over I2C to main Arduino
bool printDataflag = false;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Initialize wire library for slave I2C"));
  Wire.begin(addrSlaveI2C);    // Initiate the Wire library and join the I2C bus 
  Wire.onRequest(wireRequestEvent); // Register a function to be called when a master requests data from this slave device. 
}


void loop()
{
  I2C_Packet[0] = 0x01;
  I2C_Packet[1] = 0x02;
  I2C_Packet[2] = 0x03;
  I2C_Packet[3] = 0x04;
  I2C_Packet[4] = 0x05;
  I2C_Packet[5] = 0x06;
  I2C_Packet[6] = 0x07;
  I2C_Packet[7] = 0x08;
  I2C_Packet[8] = 0x09;
  I2C_Packet[9] = 0x10;
  
 
 if (printDataflag)
  {
    Serial.println("enviou..."); 
    printDataflag = false;
  }

}
// Send data to Master.  This is an interrupt driven event
void wireRequestEvent()
{
  // Send byte array from panStamp. Main Arduino will decode bytes
  Wire.write(I2C_Packet, PACKET_SIZE); 
  printDataflag = true;
} 
*/


