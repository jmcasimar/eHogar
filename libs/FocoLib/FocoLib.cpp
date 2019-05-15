// FocoLib.cpp
//
// Copyright (C) 2019 eHogar

#include "FocoLib.h"

Foco::Foco(int id, byte attachTo, String pub, String log, String name)
  { __Pin = attachTo ; // Pin PWM del foco
    __ID = id; // ID del foco
    __Topic = pub; // Topic MQTT al que publicará su estado
    __logTopic = log;  // Topic MQTT al que publicará logs
    __Nombre = name; // Nombre del foco o ubicación
    // Default settings
    __prevState = 0;
    __State = 0;
    __lastValue = 0;
    __lastPost = 0;
    __lastEEPROM = 0;
  }

void Foco::setup(int freq, int resolution, PubSubClient &client)
  { ledcSetup(__ID, freq, resolution);  // Setup de canal, frecuencia y resolución de PWM
    ledcAttachPin(__Pin, __ID); // Setup de pin para PWM del foco
    __State = EEPROM.read(__ID + 32); // lectura del espacio de memoria asignado para estado
    char logTopic[20];
    char logMsg[50];
    String _logMsg  = "Estado anterior de foco " + __Nombre + " es " + String(__State);  // Cadena de información
    _logMsg.toCharArray(logMsg, 50);
    __logTopic.toCharArray(logTopic, 20);
    Serial.println(logMsg); // Imprimir información en serial
    client.publish(logTopic, logMsg); // Enviar información al log del servidor
    dimm(client);
  }

void Foco::dimm(PubSubClient &client)
  { if (__State < 0){__State = 0;} // Variables de seguridad para regular de 0 a 100 %
    else if (__State > 100){__State = 100;}

    if (__State != __prevState) { // Si cambió el estado de destino
      if (__State < __prevState) { // Si el estado nuevo es menor al anterior, ir bajando
        for (int i = __prevState; i >= __State; i--)
        {
          ledcWrite(__ID, i * 2.55);
          delay(5);
        }
      }
      if (__State > __prevState) { // Si el estado nuevo es mayor al anterior, ir subiendo
        for (int i = __prevState; i <= __State; i++)
        {
          ledcWrite(__ID, i * 2.55);
          delay(5);
        }
      }
      char logTopic[20];
      char logMsg[50];
      String _logMsg  = "Foco " + __Nombre + " al " + __State + " %"; // Cadena de información
      _logMsg.toCharArray(logMsg, 50);
      __logTopic.toCharArray(logTopic, 20);
      Serial.println(logMsg); // Imprimir información en el serial
      client.publish(logTopic, logMsg); // Enviar información al log del serial
      __prevState = __State; // Asignar nuevo estado como estado previo
    }
    int now = millis() / 1000;
    if (now - __lastPost > 1) { // Si ya pasó un segundo desde la última publicación
      if (__lastValue != __State)
      {
        save(client); // Guardar valor en memoria
        publish(client); // Publicar valor al servidor
        __lastPost = now; // Guardar nuevo momento de último post
      }
    }
  }

void Foco::save(PubSubClient &client)
  { EEPROM.write(__ID + 32, __State); // Escribir en memoria
    EEPROM.commit(); // Fijar como persistente
    char logTopic[20];
    char logMsg[50];
    __logTopic.toCharArray(logTopic,20);
    String _logMsg  = "Estado de foco escrito en slot " + String(__ID + 32) + " valor: " + String(__State); // Cadena de información
    _logMsg.toCharArray(logMsg, 50);
    Serial.println(logMsg); // Imprimir en el puerto serial
    client.publish(logTopic, logMsg); // Enviar al log del servidor
  }


void Foco::publish(PubSubClient &client)
  { char topicString[20];
    char msgString[16];
    __Topic.toCharArray(topicString, 20);
    dtostrf(__State, 1, 2, msgString);
    client.publish(topicString, msgString); // Publicar estado actual en el servidor
    __lastValue = __State; // Fijar nuevo valor como último valor publicado
  }
