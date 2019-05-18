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

// autoCreator.h

#ifndef autoCreator_h
#define autoCreator_h

#include <stdlib.h>
#include <EEPROM.h>

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#define MAX_MODULES 50 // Max number of Modules that can be connected

class autoCreator
  {  private:
         byte __Number;

     public:
          static byte __TotalObjects;
          static autoCreator *ptr[MAX_MODULES] ; // List of pointers to each object
          static void autoCreation(String loc, String Type, String id, bool print); // Create new Object
          static void loadObjects(); // Load prev objects parameters
          static void cleanEeprom(); // Erase all the EEPROM parameters
          String __Location;
          String __Type;
          String __ID;
          float __Value ;

          autoCreator(String loc, String Type, String id, bool print) ; // Constructor
   } ;

   #endif
