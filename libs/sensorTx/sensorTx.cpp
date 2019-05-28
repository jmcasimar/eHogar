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

  void sensorTx::radioBegin(RFM69X& radi, int FREQUENC, int NODEI, int REMOTEI, int NETWORKI, boolean IS_RFM69H, bool SEND_PROMISCUOU)
  {
    FREQUENCY = FREQUENC;
    NETWORKID = NETWORKI;
    IS_RFM69HW = IS_RFM69H;
    SEND_PROMISCUOUS = SEND_PROMISCUOU;
    if (!radi.initialize(FREQUENCY, NODEID, NETWORKID))
    {
      Serial.println ("\n****************************************************************");
      Serial.println (" WARNING: RFM Transceiver intialization failure: Set-up Halted  ");
      Serial.println ("****************************************************************");
      while (1); // Halt the process
    }

    if (IS_RFM69HW)  radi.setHighPower();      // Only for RFM69HW!                 // Set encryption
    radi.promiscuous(SEND_PROMISCUOUS);        // Set promiscuous mode
  #ifdef RFM_SESSION
    radi.useSessionKey(SESSION_KEY);           // Set session mode
    radi.sessionWaitTime(SESSION_WAIT_WDG);    // Set the Session Wait Watchdog
    radi.useSession3Acks(SESSION_3ACKS);       // Set the Session 3 Acks option
    radi.sessionRespDelayTime(SESSION_RSP_DLY);// Set the slow node delay timer
  #endif
    Serial.print ("\nBoard Type: ");
  #ifdef __AVR_ATmega1284P__
    Serial.println ("__AVR_ATmega1284P__");
  #endif
  #ifdef __AVR_ATmega328P__
    Serial.println ("__AVR_ATmega328P__");
  #endif
  #ifdef ESP8266
    Serial.println ("ESP8266");
  #endif
  #ifdef ESP32
    Serial.println ("ESP32");
  #endif
  #ifdef __AVR_ATmega2560__
    Serial.println ("__AVR_ATmega2560__");
  #endif

  #ifdef RFM_SESSION
  #ifdef X
    Serial.println ("Using RFM69X Extended Session mode");
  #else
    Serial.println ("Using RFM69 Session mode");
  #endif
  #else
  #ifdef X
    Serial.println ("Using RFM69 Extended mode");
  #else
    Serial.println ("Using RFM69 Basic mode");
  #endif
  #endif


    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
    Serial.println(buff);

    NODEID = NODEI;
    REMOTEID = REMOTEI;
  }
