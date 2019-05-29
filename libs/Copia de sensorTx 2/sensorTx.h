/*
Library for eHogar software package
Written by :
Date : 2019-*-*
Version : 0.1 (testing version)

This file is part of eHogar.

eHogar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

eHogar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with eHogar.  If not, see <https://www.gnu.org/licenses/>.
*/

// sensorTx.h

#ifndef sensorTx_h
#define sensorTx_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <EEPROM.h>
#include <RFM69X.h>

class sensorTx
  {  private:
         String __Type;
         int __Tx;
         int __Input;
         int __Button;
         int __iType;
         unsigned long __ID;
         int now = 5;
         int last = 0;
         bool state = LOW;
         bool prevState = LOW;
         struct __attribute__((__packed__)) Payload {                // Radio packet structure max 61 bytes or 57 id SessionKey is used
           long int      nodeId; //store this nodeId
           long int      frameCnt;// frame counter
           unsigned long uptime; //uptime in ms
           int         sensorType;
           int msg;
         };
         Payload theData;
         int TRANSMITPERIOD = 500;
         long lastPeriod = -1;
         int NODEID;
         long int frameCnt = 0;
         int REMOTEID;
         byte SEND_RTRY = 0;
         unsigned long SEND_WAIT_WDG = 40;
         int message;
         long int ackSentCnt = 0;
         int FREQUENCY;
         int NETWORKID;
         char ENCRYPTKEY[16];
         bool SEND_PROMISCUOUS = false;
         byte sendSize = 0;
         int RFM_SS = 0;
         int RFM_INT = 0;
         int RFM_INTNUM = 0;
         RFM69X radio;

     public:
        sensorTx ( int ID, String type) ; // Constructor
        void begin(int input, int button, bool print) ;
        void rfTransmission(int message);
        void txConection();
        void txPairing();
        void txAlarm();
        void monitor();
        void radioBegin();
  };
   #endif
