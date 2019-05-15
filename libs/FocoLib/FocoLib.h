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

// FocoLib.h

#ifndef FocoLib_h
#define FocoLib_h

#include <stdlib.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

class Foco {
  private:
    byte __Pin; // Pin asignado
    int __ID; // ID del foco
    int __prevState;  // Estado previo del foco
    int __State;  // Estado actual del foco
    String __Topic ; // Topic MQTT para publicar
    String __logTopic ; // Topic MQTT para publicar logs
    int __lastValue;  // Variable de última publicación
    int __lastPost; // Variable para temporizador de publicaciones
    int __lastEEPROM; // Variable para último valor almacenado en memora
    String __Nombre; // Variable para nombre del foco

  public:
    Foco(int id, byte attachTo, String pub, String log, String name); // Constructor
    void setup(int freq, int resolution, PubSubClient &client);
    void dimm(PubSubClient &client);
    void save(PubSubClient &client);
    void publish(PubSubClient &client);
    int getState();
    void setState(int state);
};

#endif
