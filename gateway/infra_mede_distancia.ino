#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 4
#define RFM95_RST 2
#define RFM95_INT 3
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(4,3);
const float pi = 3.14159;


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
  if (!rf95.setFrequency(RF95_FREQ))
  {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
}

uint8_t carNode1[9];
uint8_t carNode2[9];
uint8_t buf[10];

double lat1, lat2, lon1, lon2;
double deltaLat, deltaLon;
double a, c, x, y;
double km;

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
      // Serial.print("Got: ");
      //Serial.println((char*)buf);
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);

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

      lat_carNode2 = -8.049782;//(b2fTemp.f/1000000) ;
      lng_carNode2 = -34.961814;//(b2fTempBastao.f/1000000);
      distance();
      Serial.println(km, 4);

      uint8_t data[] = "eron"; 
      
      if(km < 0.050)
      {
         // Send a reply
         //data = 57; // COLLISION!
         rf95.send(data, sizeof(data));
         Serial.println("BATEU");
         rf95.waitPacketSent();
      }
      else
     {
         // Send a reply
         //data = 56; // ok, no collision
         rf95.send(data, sizeof(data));
         rf95.waitPacketSent();
         Serial.println("NO COLISION");
     }

      //Serial.println("Sent a reply");

       
    }
    else
    {
      //Serial.println("Receive failed");
      digitalWrite(8, HIGH);
    }
  }
  // ------------ calcula a distancia entre os dois CARROs -----------------

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


void distance()
{
    lat1=radians(lat_carNode1);  lon1=radians(lng_carNode1);      // Latidude & Longitude - Carro 01
    lat2=radians(lat_carNode2);  lon2=radians(lng_carNode2);      // Latidude & Longitude - Carro 02
    deltaLat=(lat2-lat1);
    deltaLon=(lon2-lon1);
    a= ( sin((deltaLat)/2)*sin((deltaLat)/2) ) + ( cos(lat1)*cos(lat2) ) * ( sin(deltaLon/2)* sin(deltaLon/2) );
    c=2*atan2(sqrt(a),sqrt(1-a));
    x=deltaLon*cos((lat1+lat2)/2);
    y=deltaLat;
    km=6371*sqrt(x*x + y*y);  
}


