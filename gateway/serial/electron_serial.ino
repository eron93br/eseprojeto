

int inByte = -1;
char inString[6];
int stringPos = 0; 
int sensor =0;

void setup()  { 
  Serial1.begin(9600); 
  Serial.begin(9600);
  Serial.println("begin codigo.,.");
      Spark.variable("sensor", &sensor, INT);

} 

void loop() { 
   //Serial.println("-----LOOP------");
   inByte = Serial1.read();
   Serial.println(inByte);

 /*
  if((inByte >= '0') && (inByte <= '9')){
    inString[stringPos] = inByte;
    stringPos ++;
  }

  //if there is a line end character, this string is done.
  //clear the string when done
  if(inByte == '\r'){
    sensor = atoi(inString); //convert string to int
    Serial.println(sensor);
    //Spark.publish("dado",sensor);
    //clear the values from inString
    for (int c = 0; c < stringPos; c++){
      inString[c] = 0;
    }
    stringPos = 0;
  }*/
  delay(100);
}
