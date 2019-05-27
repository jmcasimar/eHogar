/*
Library for eHogar software package
Written by Jose M. Casillas, Héctor E. Martín:
Date : 2019-05-15
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

// receptorAlarma.h

#ifndef receptorAlarma_h
#define sensorTx_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#include <autoCreator.h>
#include <PubSubClient.h>
#include <RFM69X.h> //  eXteneded RFM69 base library

class receptorAlarma
  {  private:
        String __Topic;
        int __Pin;
        unsigned char rxdat[10];  // (global var) holds received RF bytes
        struct __attribute__((__packed__)) Payload {                // Radio packet structure max 61 bytes or 57 id SessionKey is used
          long int      nodeId; //store this nodeId
          long int      frameCnt;// frame counter
          unsigned long uptime; //uptime in ms
          int         sensorType;   //temperature maybe??
          int msg;
        };
        Payload theData;
        byte ackCount;
        long int pingCnt;
        byte SEND_RTRY;
        unsigned long SEND_WAIT_WDG;
        long int ackReceivedCnt;
        int sensorType;
        String ID;

     public:
        unsigned long TMR0L = 0;
        receptorAlarma (int pin, String topic) ; // Constructor
        void begin();
        void addSensor(String location, PubSubClient &client, RFM69X& radio);
        void cleanSensors(PubSubClient &client);
        void monitor(PubSubClient& client, RFM69X& radio);
        void publish(int message, PubSubClient &client);
        int receiveRF(RFM69X& radio);
        void beginRadio(byte ackCoun, long int pingCn, byte SEND_RTR, unsigned long SEND_WAIT_WD, long int ackReceivedCn);
  };
   #endif
