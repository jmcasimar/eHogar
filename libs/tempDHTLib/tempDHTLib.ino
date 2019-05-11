class tempDHT {

  private:
    int _pin;
    int ID;
    String topicTemp;
    String topicHum;
    String topicSens;
    char _topicTemp[32];
    char _topicHum[32];
    char _topicSens[32];
    int lastPost = 0;
    int lastPostError = 0;
    int _init = 0;

  public:
    float t;
    float h;
    float hic;

    tempDHT (int id, int attachTo, String temp, String hum, String sens):
      _pin(attachTo)
    {
      ID = id;
      topicTemp = temp;
      topicHum = hum;
      topicSens = sens;

      // Definimos el pin digital donde se conecta el sensor
#define DHTPIN _pin
      // Dependiendo del tipo de sensor
#define DHTTYPE DHT11
    }

    void setup() {

      // Inicializamos el sensor DHT11
      DHT dht(DHTPIN, DHTTYPE);

      dht.begin();

      topicTemp.toCharArray(_topicTemp, 32);
      topicHum.toCharArray(_topicHum, 32);
      topicSens.toCharArray(_topicSens, 32);
    }

    void read() {
      DHT dht(DHTPIN, DHTTYPE);

      int now = millis() / 1000;
      if (now - lastPost > 5) {
        // Leemos la humedad relativa
        float h = dht.readHumidity();
        // Leemos la temperatura en grados centígrados (por defecto)
        float t = dht.readTemperature();

        // Comprobamos si ha habido algún error en la lectura
        if (isnan(h) || isnan(t)) {
          
          int now = millis() / 1000;
          if (now - lastPostError > 60) {
            printLog("Error obteniendo los datos del sensor DHT11");
            lastPostError = now;
          }
          return;
        }

        // Calcular el índice de calor en grados centígrados
        float hic = dht.computeHeatIndex(t, h, false);
        printLog("La temperatura es de " + String(t) + " ºC");
        printLog("La humedad es de " + String(h) + " %");
        printLog("La sensación es de " + String(hic) + " ºC");
        publish();
        lastPost = now;
      }
    }

    void publish() {
      char msgString[16];
      dtostrf(t, 1, 2, msgString);
      client.publish(_topicTemp, msgString);
      dtostrf(h, 1, 2, msgString);
      client.publish(_topicHum, msgString);
      dtostrf(hic, 1, 2, msgString);
      client.publish(_topicSens, msgString);
    }
};
