class RGBled {

  private:
    int _pinR;  // Pin para controlar led rojo
    int _pinG;  // Pin para controlar led verde
    int _pinB;  // Pin para controlar led azul
    int _idR;   // ID de PWM para led rojo 
    int _idG;   // ID de PWM para led verde
    int _idB;   // ID de PWM para led azul
    int ID;     // ID del led

  public:
    RGBled(int id, int r, int g, int b):
      _pinR(r)
    {
      _pinG = g;
      _pinB = b;
      ID = id;
      _idR = ID + 7;
      _idG = ID + 8;
      _idB = ID + 9;
      ledcSetup(_idR, freq, resolution); // Setup de canal, frecuencia y resolución de PWMs
      ledcSetup(_idG, freq, resolution);
      ledcSetup(_idB, freq, resolution);
      ledcAttachPin(_pinR, _idR); // Setup de pines para PWM de LEDs
      ledcAttachPin(_pinG, _idG);
      ledcAttachPin(_pinB, _idB);
    }

    // Función para fijar un nuevo color de encendido
    void set(int r, int g, int b) {
      ledcWrite(_idR, r); // Escribir intensidad de rojo
      ledcWrite(_idG, g); // Escribir intensidad de verde
      ledcWrite(_idB, b); // Escribir intensidad de azul
      _logMsg  = "Valores LED: R: " + String(r) + " G: " + String(g) + " B: " + String(b); // Cadena de información
      _logMsg.toCharArray(logMsg, 50);
      Serial.println(logMsg); // Imprimir información en serial
      client.publish(logTopic, logMsg); // Enviar información al log del servidor
    }
};
