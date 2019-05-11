class Foco {
  public:

    Foco(int id, int attachTo, String pub, String name):
      pin(attachTo) // Pin de salida para PWM
    {
      ID = id; // ID del foco
      topic = pub; // Topic MQTT al que publicará su estado
      nombre = name; // Nombre del foco o ubicación
    }

    const int pin; // Pin asignado
    int ID; // ID del foco
    int prevState;  // Estado previo del foco
    int state;  // Estado actual del foco
    String topic; // Topic MQTT para publicar
    int lastValue;  // Variable de última publicación
    int lastPost = 0; // Variable para temporizador de publicaciones
    int lastEEPROM = 0; // Variable para último valor almacenado en memora
    String nombre; // Variable para nombre del foco

    void setup() {
      ledcSetup(ID, freq, resolution);  // Setup de canal, frecuencia y resolución de PWM
      ledcAttachPin(pin, ID); // Setup de pin para PWM del foco
      state = EEPROM.read(ID + 32); // lectura del espacio de memoria asignado para estado
      _logMsg  = "Estado anterior de foco " + nombre + " es " + String(state);  // Cadena de información
      _logMsg.toCharArray(logMsg, 50);
      Serial.println(logMsg); // Imprimir información en serial
      client.publish(logTopic, logMsg); // Enviar información al log del servidor
      dimm();
    }

    void dimm()
    {
      if (state < 0) state = 0; // Variables de seguridad para regular de 0 a 100 %
      else if (state > 100) state = 100;

      if (state != prevState) { // Si cambió el estado de destino
        if (state < prevState) { // Si el estado nuevo es menor al anterior, ir bajando
          for (int i = prevState; i >= state; i--)
          {
            ledcWrite(ID, i * 2.55);
            delay(5);
          }
        }
        if (state > prevState) { // Si el estado nuevo es mayor al anterior, ir subiendo
          for (int i = prevState; i <= state; i++)
          {
            ledcWrite(ID, i * 2.55);
            delay(5);
          }
        }
        _logMsg  = "Foco " + nombre + " al " + state + " %"; // Cadena de información
        _logMsg.toCharArray(logMsg, 50);
        Serial.println(logMsg); // Imprimir información en el serial
        client.publish(logTopic, logMsg); // Enviar información al log del serial
        prevState = state; // Asignar nuevo estado como estado previo
      }
      int now = millis() / 1000;
      if (now - lastPost > 1) { // Si ya pasó un segundo desde la última publicación
        if (lastValue != state)
        {
          save(); // Guardar valor en memoria
          publish(); // Publicar valor al servidor
          lastPost = now; // Guardar nuevo momento de último post
        }
      }
    }

    void save() {
      EEPROM.write(ID + 32, state); // Escribir en memoria
      EEPROM.commit(); // Fijar como persistente
      _logMsg  = "Estado de foco escrito en slot " + String(ID + 32) + " valor: " + String(state); // Cadena de información
      _logMsg.toCharArray(logMsg, 50);
      Serial.println(logMsg); // Imprimir en el puerto serial
      client.publish(logTopic, logMsg); // Enviar al log del servidor
    }

    void publish()
    {
      char topicString[20];
      char msgString[16];
      topic.toCharArray(topicString, 20);
      dtostrf(state, 1, 2, msgString);
      client.publish(topicString, msgString); // Publicar estado actual en el servidor
      lastValue = state; // Fijar nuevo valor como último valor publicado
    }

};
