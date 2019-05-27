// autoCreator.cpp
//
// Copyright (C) 2019 eHogar

#include "autoCreator.h"

byte autoCreator::__TotalObjects = 0;
autoCreator *autoCreator::ptr[MAX_MODULES];

void autoCreator::autoCreation(String loc, String Type, String id, bool print = LOW)
  {  new autoCreator(loc, Type, id, print); }

void autoCreator::loadObjects()
  { EEPROM.begin(1024);
    int totalObject = EEPROM.read(0);
    Serial.println(String(totalObject) + " objetos cargados");
    if(totalObject>0){
      for(int i=0; i<totalObject; i++){
        String loc = EEPROM.readString(i*36+1);
        String Type = EEPROM.readString(i*36+13);
        String id = EEPROM.readString(i*36+25);
        autoCreation(loc, Type, id);
      }
    }
  }

void autoCreator::cleanEeprom()
  { if(__TotalObjects>0){
        for(int i = 0; i<=__TotalObjects+i;i++){
          for(int j = 0; j<36; j++){
            EEPROM.write(i*36+j,0);
            EEPROM.commit();
          }
          ptr[i]->~autoCreator();
          ptr[i] = NULL;
        }
    }
  }

autoCreator::autoCreator(String loc, String Type, String id, bool print = LOW) // Constructor
   { // Just the first time set pointers as NULL
     if(__TotalObjects<1){
       for (int i=0; i < MAX_MODULES; i++) {
         ptr[i] = NULL;
       }
     }
     __Location = loc; // Set location
     EEPROM.writeString(__TotalObjects*36+1,__Location);
     __Type = Type; // Set module type
     EEPROM.writeString(__TotalObjects*36+13,__Type);
     __ID = id; // Set module ID
     EEPROM.writeString(__TotalObjects*36+25,__ID);
     __Value = 0 ; // Set default value
     ptr[__TotalObjects] = this; // Set Static pointer to object
     __TotalObjects++; // Add the new object to the total
     EEPROM.write(0,__TotalObjects);
     EEPROM.commit();
     if(print){
       Serial.print(F("Se ha creado un nuevo módulo en "));
       Serial.print(__Location); Serial.print(F(" del tipo "));
       Serial.print(__Type); Serial.print(F(" con el ID= "));
       Serial.println(__ID);
     }
   }

autoCreator::~autoCreator()
  { Serial.print(F("Se ha destruido el objeto correspondiente al módulo en "));
    Serial.print(__Location); Serial.print(F(" del tipo "));
    Serial.print(__Type); Serial.print(F(" con el ID= "));
    Serial.println(__ID);
    __TotalObjects--;
  }
