/*
 * 
 *             ======== MÓDULO CARRO 01 ==============
 * 
 *             ========ID_CARRO_01 = 0x55 ============
 * 
 *-------------------------------------------------------------
    Projeto ESE - módulo arduino_LoRa_TX
    Recebe informação I2C do GPS e manda por LoRa!!!!
    TAMANHO DO PACOTE I2C:    09 bytes 
    TAMANHO DO PACOTE LoRa:   xx bytes 
  *-------------------------------------------------------------
*/
#include <Wire.h>   
#include <SPI.h>    
#include <RH_RF95.h>
#define PACKET_SIZE 9
#define addrSlaveI2C 21   
#define RFM95_CS 4
#define RFM95_RST 2
#define RFM95_INT 3
#define RF95_FREQ 915.0
 
RH_RF95 rf95(4,3);

typedef struct  {
  
  float myTemp;  
  float myTempBastao;
  
                }
RemoteSensorData_t;

typedef union float2bytes_t   
                            { 
  float f; 
  char b[sizeof(float)]; 
                            }; 
// Function prototype
bool getData(RemoteSensorData_t* sensorInfo);
int velocidade;

void setup () 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  //while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  Serial.println("Arduino LoRa TX Test!");
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  delay(1000);
  Serial.println(F("Initialize library for I2C Master Test"));
  Wire.begin();  // Initialiae wire library for I2C communication
  Serial.println(F("Finished setup"));
  
} // end setup()

  float2bytes_t b2fTemp;
  float2bytes_t b2fTempBastao;

void loop()
     {
  RemoteSensorData_t sensor1; 
  
          if( getData( &sensor1 ))
                                    {
            Serial.print("\t");
            Serial.print(sensor1.myTemp);
            Serial.print("    ");
            Serial.print("\t");
            Serial.print(sensor1.myTempBastao);
            Serial.print("    ");
            Serial.print(velocidade);
            Serial.println(" "); 
            delay(100);         
            mandaLora();    
                                      }
          else
              {
            //Serial.println(F("No packet received"));
            mandaLora(); 
              }
             //delay(500);
    }  

void mandaLora()
{
    Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  char radiopacket[10] = {b2fTemp.b[0] , b2fTemp.b[1], b2fTemp.b[2], b2fTemp.b[3],  b2fTempBastao.b[0],  b2fTempBastao.b[1], 
   b2fTempBastao.b[2],  b2fTempBastao.b[3], velocidade, 0x55}; 
  //char radiopacket[20] = "Hello World #      ";
  
  //itoa(packetnum++, radiopacket+13, 10);
 // Serial.print("Sending "); Serial.println(radiopacket);
  //radiopacket[19] = 0;
  
  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)radiopacket, 20);
 
  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
 
  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(100))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
}

// I2C Request data from slave
bool getData(RemoteSensorData_t* sensorInfo)
{

  bool gotI2CPacket = false;    
  byte i=0;
  byte i2CData[PACKET_SIZE];  // don't use char data type
  
  Wire.requestFrom(addrSlaveI2C, PACKET_SIZE);    // request data from I2C slave
  
  while(Wire.available())    // Wire.available() will return the number of bytes available to read
  { 
    i2CData[i++] = Wire.read(); // receive a byte of data
    gotI2CPacket = true;  // Flag to indicate sketch received I2C packet
  }

  // If we got an I2C packet, we can extract the values
  if(gotI2CPacket)
  {
    gotI2CPacket = false;  // Reset flag
    // Get floating point number    

    b2fTemp.b[0] = i2CData[0];
    b2fTemp.b[1] = i2CData[1];
    b2fTemp.b[2] = i2CData[2];
    b2fTemp.b[3] = i2CData[3];
    sensorInfo->myTemp = b2fTemp.f;
    
    b2fTempBastao.b[0] = i2CData[4];
    b2fTempBastao.b[1] = i2CData[5];
    b2fTempBastao.b[2] = i2CData[6];
    b2fTempBastao.b[3] = i2CData[7];
    sensorInfo->myTempBastao = b2fTempBastao.f;

    velocidade = i2CData[8]; 
     
    return true;
  }  // end got packet

  else
  
  { 
    return false; 
    
    } // No Packet received
  
} // end getData
