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

  void sensorTx::radioBegin()
  {

    boolean IS_RFM69HW = false; // If High power RFM69HW is used

    #ifdef ESP32                // Allows automatic ESP32 processor recognition
    #include <WiFi.h>          // Required for ESP32
    #define X                  // eXtended Library Selector mandatory for ESP32
    #endif
    #include <RFM69X.h> //  eXteneded RFM69 base library

    #define NODEID      99
    #define NETWORKID   100
    #define REMOTEID   1
    bool SEND_PROMISCUOUS = false;       // Set to 'true' to sniff all packets on the same network

    #define FREQUENCY     RF69_915MHZ
    #define ENCRYPTKEY   "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!

    // Adapt configuration against the processor Type
    #ifdef __AVR_ATmega1284P__
    #define LED         15        // Moteino MEGA have LED on D15
    #define RFM_SS      4         // Default Slave Select Moteino MEGA
    #define RFM_INT     2         // One if a free GPIO supporting Interrupts
    #endif
    #ifdef __AVR_ATmega328P__
    #define LED         9         // Moteino has LEDs on D9
    #define RFM_SS      10        // Default Slave Select for Arduino UNO or Moteino
    #define RFM_INT     2         // One if a free GPIO supporting Interrupts
    #endif
    #ifdef ESP32
    #define LED         2         // LED is on pin 2 for ESP-32
    #define RFM_SS      SS         // Default Slave Select PIN 5 for LOLIN 32
    #define RFM_INT     4        // One if a free GPIO supporting Interrupts
    #endif

    #ifdef __AVR_ATmega1284P__
    byte RFM_INTNUM = 2; // digitalPinToInterrupt doesn't work for ATmeg1284p
    #else
    byte RFM_INTNUM = digitalPinToInterrupt(RFM_INT);  // Standard way to convert Interrupt pin in interrupt number
    #endif

    radio = new  RFM69X(RFM_SS, RFM_INT, IS_RFM69HW, RFM_INTNUM);

    FREQUENCY = FREQUENC;
    NETWORKID = NETWORKI;
    IS_RFM69HW = IS_RFM69H;
    SEND_PROMISCUOUS = SEND_PROMISCUOU;
    if (!radio.initialize(FREQUENCY, NODEID, NETWORKID))
    {
      Serial.println ("\n****************************************************************");
      Serial.println (" WARNING: RFM Transceiver intialization failure: Set-up Halted  ");
      Serial.println ("****************************************************************");
      while (1); // Halt the process
    }

    if (IS_RFM69HW)  radio.setHighPower();      // Only for RFM69HW!                 // Set encryption
    radio.promiscuous(SEND_PROMISCUOUS);        // Set promiscuous mode
  #ifdef RFM_SESSION
    radio.useSessionKey(SESSION_KEY);           // Set session mode
    radio.sessionWaitTime(SESSION_WAIT_WDG);    // Set the Session Wait Watchdog
    radio.useSession3Acks(SESSION_3ACKS);       // Set the Session 3 Acks option
    radio.sessionRespDelayTime(SESSION_RSP_DLY);// Set the slow node delay timer
    radio.encrypt(ENCRYPTKEY);

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


void sensorTx::rfTransmission(int message)
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

void sensorTx::txConection() {
    message = 20;
    rfTransmission(message);
  }

void sensorTx::txPairing() {
  message = 21;
  rfTransmission(message);
  }

void sensorTx::txAlarm() {
  if (prevState != state) {
    if (state)  message = 22;
    else message = 23;
  rfTransmission(message);
  prevState = state;
  }
  }

void sensorTx::monitor() {
    now = millis()/1000-last;
    if(digitalRead(__Input)) state = HIGH;
    else state = LOW;
    txAlarm();
    if(digitalRead(__Button)) txPairing();
    if (now > 5) {
      txConection();
      last = millis()/1000;
    }
  }
