class Timbre {

  private:
    const int _pin;

  public:
    Timbre(int id, int pin, String name):
      _pin(pin)
    {
      ID = id;
      pinMode(_pin, INPUT);
      nombre = name;
    }
    int ID;
    String nombre;
    int read()
    {
      if (digitalRead(_pin))
      {
        _logMsg  = "Timbre " + nombre + " presionado";
        _logMsg.toCharArray(logMsg, 50);
        Serial.println(logMsg);
        client.publish(logTopic, logMsg);
        while (digitalRead(_pin))
        {}
        delay(100);
        return 1;
      }
      else return 0;
    }
};
