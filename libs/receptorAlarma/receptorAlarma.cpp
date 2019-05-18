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
     int objects = autoCreator::loadObjects();
     Serial.print(F("Se han cargado "));
     Serial.print(autoCreator::__TotalObjects);
     Serial.println(F(" sensores."));
   }


void receptorAlarma::addSensor(String location)
  { unsigned long message = 0;
    unsigned long message2 = 0;
    int now = millis()/1000;
    int tim = millis()/1000;
    bool check = 0;
    while(!message && now-tim < 15) {
     now = millis()/1000;
     message = receiveRF();
     Serial.println(String(message));
     if(message){
       while(!message2 && now-tim < 15) {
         now = millis()/1000;
         message2 = receptorAlarma::receiveRF();
         if (message == message2) {
           String type = String(message / 1000000);
           String ID = String((message % 1000000)/100);
           String sType;
           if (type == "10") sType = "movimiento";
           String alarm =  String(message % 100);
           Serial.println(String(type) + String(ID) + String(alarm));
           if (alarm == "21") {
             for (int i = 0; i < autoCreator::__TotalObjects; i++){
               if (ID == autoCreator::ptr[i]->__ID) {
                 Serial.println("Este sensor ya había sido añadido");
                 return;
               }
             }
             autoCreator::autoCreation(location, sType, ID, HIGH);
             check = 1;
             Serial.println("Sensor agregado");
             //client.publish(__Topic, "agregado");
           }
         }
       }
     }
    }
    if (!check) int a = 1; //client.publish(__Topic, "error");
  }

void receptorAlarma::monitor()
  { unsigned long message = receiveRF();
    publish(message);
  }

void receptorAlarma::publish(unsigned long message, PubSubClient &client)
  {
    if (message) {
      String ID = String((message % 1000000)/100);
      int alarm =  (message % 100);
      String alarmType;
      if (alarm == 20) alarmType = "conectado";
      else if (alarm == 21) alarmType = "sincronizando";
        else if (alarm == 22) alarmType = "activado";
        Serial.println("ID: " + ID + " Alarma: " + alarmType);

      for (int i = 0; i < autoCreator::__TotalObjects; i++){
        if (ID == autoCreator::ptr[i]->__ID) {
          String type =  autoCreator::ptr[i]->__Type;
          String location = autoCreator::ptr[i]->__Location;
          Serial.print(F("El sensor de ")); Serial.print(type); Serial.print(F(" con ID ")); Serial.print((ID));
          Serial.print(F(" ha transmitido en ")); Serial.print((location)); Serial.print(F(" como ")); Serial.println((alarmType));
          String mqttMessage;
          mqttMessage = String(type) + "/" + String(location) + "/" + String(alarmType);
          Serial.println("El mensaje MQTT es " + mqttMessage);
          client.publish(__Topic, mqttMessage)
        }
      }
    }
  }

unsigned long receptorAlarma::receiveRF() {
  unsigned long rrp_data;
  unsigned char rrp_period;
  unsigned char rrp_bits;
  unsigned char rrp_bytes;
  unsigned long tim = 0;
  unsigned long now = 0;

  rrp_bytes = 0;
  tim = millis();
  now = millis();
  while(rrp_bytes < 10)   // loop until it has received 10 contiguous RF bytes
  {
    now = millis();
    if (now-tim > 100) return 0;
    //-----------------------------------------
    // wait for a start pulse >200uS
    while(1)
    {
      now = millis();
    if (now-tim > 100) return 0;
      while(!digitalRead(__Pin)) {
        now = millis();
    if (now-tim > 100) return 0;
      }    // wait for input / edge
      while(digitalRead(__Pin)) {
        now = millis();
    if (now-tim > 100) return 0;
      }     // wait for input \ edge
      rrp_period = TMR0L;           // grab the pulse period!
      //Serial.println(TMR0L);
      TMR0L = 0;                    // and ready to record next period
      if(rrp_period < 200) rrp_bytes = 0;   // clear bytecount if still receiving noise
      else break;                   // exit if pulse was >200uS
    }

    //-----------------------------------------
    // now we had a start pulse, record 32 bits
    rrp_bits = 32;
    while(rrp_bits)
    {
      while(!digitalRead(__Pin)) {}    // wait for input / edge
      while(digitalRead(__Pin)) {}     // wait for input \ edge
      rrp_period = TMR0L;           // grab the pulse period!
      //Serial.println(TMR0L);
      TMR0L = 0;                    // and ready to record next period

      if(rrp_period >= 200) break;  // if >=200uS, is unexpected start pulse!

      if(rrp_period < 122) {
        bitWrite(rrp_data, 32-rrp_bits, 0);    // 122uS
        //Serial.print("0");
      }
      else {
        bitWrite(rrp_data, 32-rrp_bits, 1);
        //Serial.print("1");
      }
        rrp_bits--;                   // and record 1 more good bit done
    }

    //-----------------------------------------
    // gets to here after 8 good bits OR after an error (unexpected start pulse)
    if(rrp_bits)      // if error
    {
      rrp_bytes = 0;  // reset bytes, must run from start of a new packet again!
      return 0;
    }
    else              // else 8 good bits were received
    {
      Serial.println(" ");
      Serial.println("Mensaje RF: " + String(rrp_data));
      rxdat[rrp_bytes] = rrp_data;  // so save the received byte into array
      rrp_bytes++;                  // record another good byte was saved
      return rrp_data;
    }
    return 0;
  }
  return 0;
}
