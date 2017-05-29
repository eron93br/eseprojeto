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

#include <Wire.h>       
#define PACKET_SIZE 8   
#define addrSlaveI2C 21   

// Function prototype
bool getData(RemoteSensorData_t* sensorInfo);

   void setup () 
              {
  Serial.begin(9600);
  delay(2000);
  Serial.println(F("Initialize library for I2C Master Test"));
  Wire.begin();  // Initialiae wire library for I2C communication
  Serial.println(F("Finished setup"));
  
               } // end setup()


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
            Serial.println(" "); 
            delay(500);            
                                      }
          else
              {
            Serial.println(F("No packet received"));
              }
             //delay(500);
    }  


// I2C Request data from slave
bool getData(RemoteSensorData_t* sensorInfo)
{
  float2bytes_t b2fTemp;
  float2bytes_t b2fTempBastao;
  
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

    return true;
  }  // end got packet

  else
  
  { 
    return false; 
    
    } // No Packet received
  
} // end getData

