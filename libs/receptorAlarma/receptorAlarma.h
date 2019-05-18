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

class receptorAlarma
  {  private:
        String __Topic;
        int __Pin;
        unsigned char rxdat[10];  // (global var) holds received RF bytes

     public:
        unsigned long TMR0L = 0;
        receptorAlarma (int pin, String topic) ; // Constructor
        void begin();
        void addSensor(String location, PubSubClient &client);
        void monitor(PubSubClient &client);
        void publish(unsigned long message, PubSubClient &client);
        unsigned long receiveRF();
  };
   #endif
