class Proximidad {

  private:
    const int _pin;

  public:
    Proximidad(int id, int pin, String name):
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
      if (analogRead(_pin) < 4000)
      {
        printLog("Sensor de proximidad " + nombre + " presionado");
        while (analogRead(_pin) < 4000)
        {}
        delay(100);
        return 1;
      }
      else return 0;
    }
};
