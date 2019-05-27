// sensorTx.cpp
//
// Copyright (C) 2019 eHogar

#include "receptorAlarma.h"

receptorAlarma::receptorAlarma(int pin, String topic) // Constructor
   {__Pin = pin;
    __Topic = topic; // Set MQTT alarm topic

   }

void receptorAlarma::begin()
   {
     Serial.println(F("Inicializando sensores"));
     autoCreator::loadObjects();
   }


void receptorAlarma::addSensor(String location, PubSubClient &client, RFM69X& radio)
  { unsigned long message = 0;
    unsigned long message2 = 0;
    String id1;
    String id2;
    int now = millis()/1000;
    int tim = millis()/1000;
    bool check = 0;
    char topic[20];
    char msg[20];
    char msg1[20];
    String agregado = "agregado";
    String error = "error";
    __Topic.toCharArray(topic,20);
    agregado.toCharArray(msg, 20);
    error.toCharArray(msg1, 20);
    while(!message && now-tim < 15) {
     now = millis()/1000;
     message = receiveRF(radio);
     Serial.println(String(message));
     if(message){
       id1 = ID;
       while(!message2 && now-tim < 15) {
         now = millis()/1000;
         message2 = receptorAlarma::receiveRF(radio);
         id2 = ID;
         if (message == message2 && id1 == id2) {
           String sType;
           if (sensorType == 10) sType = "movimiento";
           Serial.println(String(sensorType) + String(ID) + String(message));
           if (message == 21) {
             for (int i = 0; i < autoCreator::__TotalObjects; i++){
               if (ID == autoCreator::ptr[i]->__ID) {
                 Serial.println("Este sensor ya había sido añadido");
                 return;
               }
             }
             autoCreator::autoCreation(location, sType, ID, HIGH);
             check = 1;
             Serial.println("Sensor agregado");
             client.publish(topic, msg);
           }
         }
       }
     }
    }
    if (!check) int a = 1; client.publish(topic, msg1);
  }

void receptorAlarma::cleanSensors(PubSubClient& client)
  { autoCreator::cleanEeprom(); }

void receptorAlarma::monitor(PubSubClient& client, RFM69X& radio)
  { int message = receiveRF(radio);
    publish(message, client);
  }

void receptorAlarma::publish(int message, PubSubClient &client)
  {
    if (message) {
      int alarm =  message;
      String alarmType;
      if (alarm == 20) alarmType = "conectado";
      else if (alarm == 21) alarmType = "sincronizando";
        else if (alarm == 22) alarmType = "activado";
        Serial.println("ID: " + ID + " Alarma: " + alarmType);

      for (int i = 0; i < autoCreator::__TotalObjects; i++){
        if (ID == autoCreator::ptr[i]->__ID) {
          char mqttTop[20];
          char mqttMsg[60];
          String type =  autoCreator::ptr[i]->__Type;
          String location = autoCreator::ptr[i]->__Location;
          Serial.print(F("El sensor de ")); Serial.print(type); Serial.print(F(" con ID ")); Serial.print((ID));
          Serial.print(F(" ha transmitido en ")); Serial.print((location)); Serial.print(F(" como ")); Serial.println((alarmType));
          String mqttMessage;
          mqttMessage = String(type) + "/" + String(location) + "/" + String(alarmType);
          __Topic.toCharArray(mqttTop, 20);
          mqttMessage.toCharArray(mqttMsg, 60);
          Serial.println("El mensaje MQTT es " + mqttMessage);
          client.publish(mqttTop, mqttMsg);
        }
      }
    }
  }

int receptorAlarma::receiveRF(RFM69X& radio) {
  int now = millis();
  if (radio.receiveDone())
  {
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    Serial.print(" [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");

    if (radio.DATALEN != sizeof(Payload))
      Serial.print("Invalid payload received, not matching Payload struct!");
    else
    {
      theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
      Serial.print(" nodeId=");
      Serial.print(theData.nodeId);
      Serial.print(" frameCount=");
      Serial.print (theData.frameCnt);
      Serial.print(" uptime=");
      Serial.print(theData.uptime);
      Serial.print(" sensorType=");
      Serial.print(theData.sensorType);
      Serial.print(" msg=");
      Serial.print(theData.msg);
      sensorType = theData.sensorType;
      ID = String(theData.nodeId);
    }

    if (radio.ACKRequested())
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
      Serial.print(" - ACK sent.");

      // When a node requests an ACK, respond to the ACK
      // and also send a packet requesting an ACK (every 3rd one only)
      // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
      if (ackCount++%3==0)
      {
        pingCnt++;
        Serial.print(" Pinging node ");
        Serial.print(theNodeID);
        Serial.print(" Ping count= ");
        Serial.print (pingCnt);
        delay(10); //need this when sending right after reception .. ?
        if (radio.sendWithRetry(theNodeID, "ACK TEST", SEND_RTRY, SEND_WAIT_WDG))
        {
          Serial.print (" ACK received = ");
          Serial.print (++ackReceivedCnt);
        }
        else Serial.print(" nothing");
      }
    }
    Serial.println();
    return theData.msg;
  }
  delay(1);
  return 0;
}

void receptorAlarma::beginRadio(byte ackCoun, long int pingCn, byte SEND_RTR, unsigned long SEND_WAIT_WD, long int ackReceivedCn) {
ackCount = ackCoun;
pingCnt = pingCn;
SEND_RTRY = SEND_RTR;
SEND_WAIT_WDG = SEND_WAIT_WD;
ackReceivedCnt = ackReceivedCn;
}
