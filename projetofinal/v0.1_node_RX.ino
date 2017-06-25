// Arduino9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Arduino9x_TX
 
#include <SPI.h>
#include <RH_RF95.h>
#include <TinyGPS++.h>

 
#define RFM95_CS 4
#define RFM95_RST 2
#define RFM95_INT 3
 
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Create a TinyGPS++ object called "gps"
TinyGPSPlus gps;
 
// Blinky on receipt
#define LED 13
 
void setup() 
{
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  //while (!Serial);
  Serial.begin(9600);
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
  if (!rf95.setFrequency(RF95_FREQ)) 
  {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
 
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

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

Float2bytes_t b2fTemp;
Float2bytes_t b2fTempBastao;
RemoteSensorData_t* sensorInfo;

double lat_carNode1; double lng_carNode1; int vCarro1;
double lat_carNode2; double lng_carNode2; int vCarro2;

uint8_t carNode1[9];
uint8_t carNode2[9];
uint8_t buf[20];
double distancia=0;
 
void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      // ------->>>> Recebe do LoRa! <<<<-------------------
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
       Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      
      //  --------->>>> Send a reply <<<<<------------------
      uint8_t data[] = "SIM!";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(LED, LOW);

      // ------------ALCULA DISTANCIA EM METROS -------------
      if(buf[9] == 0x55)                  // 0x55 é o CARRO 1
      {
         ajeita();  // organiza variaveis 
         lat_carNode1 = (b2fTemp.f/1000000) ;
         lng_carNode1 = (b2fTempBastao.f/1000000);
      }
      else if( buf[9] == 0xAA)            // 0xAA é o CARRO 2
      {
        ajeita();  // 
        lat_carNode2 = -8.049782;//(b2fTemp.f/1000000) ;
        lng_carNode2 = -34.961814;//(b2fTempBastao.f/1000000);
      }
 
      distancia = gps.distanceBetween(-8.049549, -34.961959,  -8.04976, -34.961605);
      // --------------------------------------------------------
      Serial.println(distancia); 
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}

void ajeita()
{
    // b2fTemp.f/1000000  é a LATITUDE!
    b2fTemp.b[0] = buf[0];
    b2fTemp.b[1] = buf[1];
    b2fTemp.b[2] = buf[2];
    b2fTemp.b[3] = buf[3];
    // b2fTempBastao.f/1000000 é a LONGITUDE!
    b2fTempBastao.b[0] = buf[4];
    b2fTempBastao.b[1] = buf[5];
    b2fTempBastao.b[2] = buf[6];
    b2fTempBastao.b[3] = buf[7];
}


// distanceBetween(double lat1, double long1, double lat2, double long2)
