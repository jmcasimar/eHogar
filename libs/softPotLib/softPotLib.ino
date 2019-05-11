class softPot {

  private:
    const int _pin;
    int prevState;
    int lastPost = 0;

  public:
    Pot(int id, int pin, String name):
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
      int now = millis();
      if (now - lastPost > 500) {
        int state = map(analogRead(_pin), 1000, 3000, 0 , 100);
        if (state > -15 && state < 0) state = 0;
        else if (state < 140 && state > 100) state = 100;
        if (state >= 0 && state <= 100) {
          if (state != prevState)
          {
            printLog("Pot " + nombre + " al " + String(state) + "%");
            prevState = state;
            return state;
          }

        }
        lastPost = now;
      }
      else return -1;
    }

};
