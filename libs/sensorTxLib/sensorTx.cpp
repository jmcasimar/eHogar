// sensorTx.cpp
//
// Copyright (C) 2019 eHogar

#include "sensorTx.h"

sensorTx::sensorTx( int ID, String type) // Constructor
   {
    __ID = ID; // Set module ID
    __Type = type; // Set module type
   }

void sensorTx::begin(int tx, int input, int button, bool print = LOW)
  { __Tx = tx; // Set TX pin
    __Input = input; // Set sensor input pin
    __Button = button; // Set button input pin

    pinMode(__Tx, OUTPUT);
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
       Serial.print(__Input); Serial.print(F(" con pin de Tx "));
       Serial.print(__Tx); Serial.print(F(" con pin de botón "));
       Serial.println(__Button);
     }
     
     txConection();

  }

void sensorTx::rfTransmission(unsigned long txdat)
  {
    Serial.println("Transmitiendo mensaje RF: " + String(txdat));
    for (int i = 0; i < 2; i++) {
      unsigned char tbit;

      // make 250uS start bit first
      digitalWrite(__Tx,LOW);
      delayMicroseconds(170*5);      // 170uS LO
      digitalWrite(__Tx,HIGH);
      delayMicroseconds(80*5-1);     // 80uS HI
      digitalWrite(__Tx,LOW);

      // now loop and send 16 bits
      for(int tbit=0; tbit<32; tbit++)
      {
        delayMicroseconds(20*5-1);             // default 0 bit LO period is 20uS
        bool bitt = bitRead(txdat, tbit);
        if(bitt){
          delayMicroseconds(50*5);
          } // increase the LO period if is a 1 bit!
        digitalWrite(__Tx,HIGH);
        delayMicroseconds(80*5-1);             // 80uS HI pulse
        digitalWrite(__Tx,LOW);
      }
    }
  }

void sensorTx::txConection() {
    unsigned long message = __iType * 1000000;
    message += __ID * 100;
    message += 20;
    rfTransmission(message);
  }

void sensorTx::txPairing() {
  unsigned long message = __iType * 1000000;
  message += __ID * 100;
  message += 21;
  rfTransmission(message);
  }

void sensorTx::txAlarm() {
  if (prevState != state) {
  unsigned long message = __iType * 1000000;
  message += __ID * 100;
  message += 22;
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
