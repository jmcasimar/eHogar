class Puerta {
  private:
    String topic;
    int ID;
    int _pin;
    char topicString[20];

    public:
    int state = 0;
    String nombre;
    String topic;
    Puerta(int id, String pub, int pin, String pub, String name):
    _pin(pin)
    {
      ID = id;
    }

    void setup() {
       topic = pub;
       topic.toCharArray(topicString, 20);
       pinMode(_pin, OUTPUT);
       nombre = name;
       topic = pub;
    }
  
    void abrir() {
      state = 1;
      delay(1000);
      _logMsg  = "Puerta " + nombre + " abierta";
      _logMsg.toCharArray(logMsg, 50);
      digitalWrite(_pin, HIGH);
      client.publish(topicString, "abierta");
      Serial.println(logMsg);
      client.publish(logTopic, logMsg);
      delay(1000);
      digitalWrite(_pin, LOW);
    }
    
     void cerrar() {
      state = 0;
      // Enviar orden de cerrado
      client.publish(topicString, "cerrando");
      Serial.println("Puerta " + String(ID) + " cerrando");
      
      client.publish(topicString, "cerrada");
      Serial.println("Puerta " + String(ID) + " cerrada");
      
    }
  
};

