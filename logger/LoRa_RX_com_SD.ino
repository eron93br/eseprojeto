#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <RH_RF95.h>

#define RFM95_CS 4
#define RFM95_RST 2
#define RFM95_INT 3
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(4,3);

File myFile01;
File myFile02;

typedef struct 
{
  float myTemp;  
  float myTempBastao;
 }
RemoteSensorData_t;

typedef union float2bytes_t
{ 
  float f; 
  char b[sizeof(float)]; 
}Float2bytes_t;


// Blinky on receipt
#define LED 13

Float2bytes_t b2fTemp;
Float2bytes_t b2fTempBastao;
RemoteSensorData_t* sensorInfo;

void setup()
{
  // SETUP DO LORA
  Serial.begin(9600);
  pinMode(LED, OUTPUT);     
  pinMode(8,OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);
  Serial.println("Arduino LoRa RX Test!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init())
  {
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
  rf95.setTxPower(23, false);

  // setup do SD
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) 
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

uint8_t carNode1[9];
uint8_t carNode2[9];
uint8_t buf[10];

float lat_carNode1; float lng_carNode1; int vCarro1;
float lat_carNode2; float lng_carNode2; int vCarro2;

void loop()
{
  digitalWrite(8, HIGH);
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      digitalWrite(8, LOW);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      //Serial.println((char*)buf);
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);

      if(buf[9] == 0x55)                  // 0x55 é o CARRO 1
      {
         ajeita();  // organiza variaveis 
         escreve1();
      }
      else if( buf[9] == 0xAA)            // 0xAA é o CARRO 2
      {
        ajeita();  // 
        escreve2(); 
      }

      
      // Send a reply
      uint8_t data[] = "RX ok";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("Receive failed");
      digitalWrite(8, HIGH);
    }
  }
}

void escreve1()
{
    myFile01 = SD.open("test1.txt", FILE_WRITE);
   if (myFile01) 
    {
       Serial.println("escrevendo..1.");
       myFile01.println(" ");
       myFile01.print(b2fTemp.f/1000000, 6);
       myFile01.print(" , ");
       myFile01.print(b2fTempBastao.f/1000000, 6);
       myFile01.print(" , ");
       myFile01.print(buf[8]);
      // close the file:
       myFile01.close();          
     }
}

void escreve2()
{
      myFile02 = SD.open("test2.txt", FILE_WRITE);
   if (myFile02) 
    {
       Serial.println("escrevendo..2.");
       myFile02.println(" ");
       myFile02.print(b2fTemp.f/1000000, 6);
       myFile02.print(" , ");
       myFile02.print(b2fTempBastao.f/1000000, 6);
       myFile02.print(" , ");
       myFile02.print(buf[8]);
      // close the file:
       myFile02.close();          
     }
}

void ajeita()
{
    b2fTemp.b[0] = buf[0];
    b2fTemp.b[1] = buf[1];
    b2fTemp.b[2] = buf[2];
    b2fTemp.b[3] = buf[3];
    
    b2fTempBastao.b[0] = buf[4];
    b2fTempBastao.b[1] = buf[5];
    b2fTempBastao.b[2] = buf[6];
    b2fTempBastao.b[3] = buf[7];
    //sensorInfo->myTemp = b2fTemp.f;
     // Serial.println(b2fTemp.f/1000000, 6);
    //Serial.println(b2fTempBastao.f/1000000, 6);
    //Serial.println(sensorInfo->myTemp);
}
