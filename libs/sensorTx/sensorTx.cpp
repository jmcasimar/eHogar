// sensorTx.cpp
//
// Copyright (C) 2019 eHogar

#include "sensorTx.h"

sensorTx::sensorTx( int ID, String type) // Constructor
   {
    __ID = ID; // Set module ID
    __Type = type; // Set module type
   }

void sensorTx::begin(int input, int button, bool print = LOW)
  {
    __Input = input; // Set sensor input pin
    __Button = button; // Set button input pin

    pinMode(__Input, INPUT);
    pinMode(__Button, INPUT_PULLUP);

    if (__Type == "movimiento") __iType = 10;
    else if (__Type == "vibracion") __iType = 11;
    else if (__Type == "presencia") __iType = 12;
    else if (__Type == "humo") __iType = 13;

    if(print){
       Serial.print(F("Se ha creado  el sensor "));
       Serial.print(__ID); Serial.print(F(" del tipo "));
       Serial.print(__Type); Serial.print(F(" con pin de entrada "));
       Serial.print(__Input); Serial.print(F(" con pin de botón "));
       Serial.println(__Button);
     }

  }

void sensorTx::rfTransmission(int message, RFM69X& radio)
  {
    if (radio.receiveDone())
  {
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");

    if (radio.ACKRequested())
    {
      ackSentCnt++;
      radio.sendACK();
      Serial.print(" - ACK sent= ");
      Serial.println (ackSentCnt);
      delay(10);
    }
    Serial.println();
  }

  int currPeriod = millis()/TRANSMITPERIOD;
  if (currPeriod != lastPeriod)
  {
    //fill in the struct with new values
    theData.nodeId = NODEID;
    theData.frameCnt = ++frameCnt;
    theData.uptime = millis();
    theData.sensorType = __iType;
    theData.msg = message;

    Serial.print("Sending struct (");
    Serial.print(sizeof(theData));
    Serial.print(" bytes) ");
    Serial.print(" frame Count= ");
    Serial.print(theData.frameCnt);
    Serial.print(" uptime= ");
    Serial.print(theData.uptime);
    Serial.print(" .... ");
    if (radio.sendWithRetry(REMOTEID, (const void*)(&theData), sizeof(theData),SEND_RTRY,SEND_WAIT_WDG))
      Serial.print(" ok!");
    else Serial.print(" nothing...");
    Serial.println();
    lastPeriod=currPeriod;
  }
  }

void sensorTx::txConection(RFM69X& radi) {
    message = 20;
    rfTransmission(message,radi);
  }

void sensorTx::txPairing(RFM69X& radi) {
  message = 21;
  rfTransmission(message,radi);
  }

void sensorTx::txAlarm(RFM69X& radi) {
  if (prevState != state) {
    if (state)  message = 22;
    else message = 23;
  rfTransmission(message,radi);
  prevState = state;
  }
  }

void sensorTx::monitor(RFM69X& radi) {
    now = millis()/1000-last;
    if(digitalRead(__Input)) state = HIGH;
    else state = LOW;
    txAlarm(radi);
    if(digitalRead(__Button)) txPairing(radi);
    if (now > 5) {
      txConection(radi);
      last = millis()/1000;
    }
  }

  void sensorTx::radioBegin(RFM69X& radi, long int ackSentCn, int TRANSMITPERIO, long lastPerio, int NODEI, long int frameCn, int REMOTEI, byte SEND_RTR, unsigned long SEND_WAIT_WD)
  {
    ackSentCnt = ackSentCn;
    TRANSMITPERIOD = TRANSMITPERIO;
    lastPeriod = lastPerio;
    NODEID = NODEI;
    frameCnt = frameCn;
    REMOTEID = REMOTEI;
    SEND_RTRY = SEND_RTR;
    SEND_WAIT_WDG = SEND_WAIT_WD;
  }
