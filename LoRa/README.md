# códigos gerais

- Duplex

  LoRa Duplex communication

  Sends a message every half second, and polls continually for new incoming messages. Implements a one-byte addressing scheme, with 0xFF as the broadcast address.

void loop() 
{
  digitalWrite(8, LOW);
  if (millis() - lastSendTime > interval) 
  {
    String a = String(12);
    String b = String(255);
    // String message = String( a + b + a);
    uint8_t message[4] = {'m', 'a', 'g', 'o'};
    //String message = "HeLoRa World!";   // send a message
    sendMessage(message);
    //Serial.println("Sending " + message);
    lastSendTime = millis();            // timestamp the message
    interval = random(2000) + 1000;    // 2-3 seconds
  }
  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void sendMessage(uint8_t* outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(4);        // add payload length
  LoRa.write(outgoing,4);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}
