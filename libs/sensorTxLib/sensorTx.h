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

class sensorTx
  {  private:
         String __Type;
         int __Tx;
         int __Input;
         int __Button;
         unsigned long __iType;
         unsigned long __ID;
         int now = 5;
         int last = 0;
         bool state = LOW;
         bool prevState = LOW;


     public:
        sensorTx ( int ID, String type) ; // Constructor
        void begin( int tx, int input, int button, bool print) ;
        void rfTransmission(unsigned long txdat);
        void txConection();
        void txPairing();
        void txAlarm();
        void monitor();
  };
   #endif
