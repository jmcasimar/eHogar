#include <EEPROM.h>
#include <Wiegand.h>

WIEGAND wg;
class rfidWiegand {
  private:
    String topic;
    int ID;
    int _pin;
    char topicString[20];
    int pinD0;
    int pinD1;
    long pass[10] = {0,0,0,0,0,0,0,0,0,0};
    int keyID;
    long code;
    long code2;

  public:
    String nombre;
    rfidWiegand(int id, String pub, int D0, int D1, String name):
      pinD0(D0)
    {
      ID = id;
      pinD0 = D0;
      pinD1 = D1;
      topic = pub;
      nombre = name;
    }

    void setup() {
      topic.toCharArray(topicString, 20); // Crear nombre del topic para publicar
      wg.begin(pinD0, pinD1); // Inicializar lector RFID
      keyID = EEPROM.read(128); // Leer el número de llaves registradas
      for (int i=0; i<=keyID; i++) {
        pass[i] = EEPROM.read(i*12);
      }
    }

    // Función para leer y comparar tarjetas
    int leer() {
      if (wg.available()) // Si hay una tarjeta disponible
      { 
        long code = wg.getCode(); // Leer código
        Serial.print("Wiegand HEX = "); // Imprimir códigos y tipo de tarjeta
        Serial.print(code, HEX);
        Serial.print(", DECIMAL = ");
        Serial.print(String(code));
        Serial.print(", Type W");
        Serial.println(wg.getWiegandType());
      }
      int check = 0; // Inicializar variable de control de coincidencias
      for (int i=0; i<=keyID; i++) { // Revisar las claves conocidas
        if (code == pass[i]) check = 1;
      }
      if (check) // Si el código leído es igual al conocido
      { 
        client.publish(topicString,"reconocida");
        printLog("Tarjeta reconocida en acceso " + nombre);
        return 1; // Regresar un 1 para abrir la puerta
      }
      else // Si no es un código conocido
      {
        client.publish(topicString,"desconocida");
        printLog("Tarjeta no reconocida en acceso " + nombre);
        return 0; // Regresar un 0
      }
    }

    // Función para registrar tarjetas nuevas
    int registrar() {
      printLog("Entrando en modo de registro de llaveros");
      if (keyID <= 10) // Si contamos con menos de 10 llaveros registrados
      {
        code2 = 1; // Reiniciar la segunda lectura
        client.publish(topicString,"llavero");
        printLog("Solicitando llavero en acceso " + nombre);
        int timmer = millis() / 1000;
        int now = millis() / 1000;
        while (!wg.available()  && now - timmer < 15) // Esperar 15 segundos por un llavero
        {
          now = millis() / 1000;
        }
        if (wg.available()) // Si hay llavero disponible
        {
          code = wg.getCode(); // Leer y almacenar código

          // Solicitar segunda lectura de llavero
          client.publish(topicString,"llavero2");
          printLog("Solicitando nuevamente llavero en acceso " + nombre);
          timmer = millis() / 1000;
          int now = millis() / 1000;
          while (!wg.available()  && now - timmer < 15)
          {
            now = millis() / 1000;
          }
          if (wg.available())
          {
            code2 = wg.getCode();
          }
        }
        if (code == code2) // Si los códigos de ambas lecturas coinciden, registrar
        {
          pass[keyID] = code; // Variable de almacenamiento del código
          EEPROM.write(keyID * 12, pass[keyID]); // Asigna 12 espacios para guardar
          keyID = + keyID;
          EEPROM.write(128, keyID);
          EEPROM.commit();
          client.publish(topicString,"agregado");
          printLog("Llavero añadido en acceso " + nombre + ", keyID: " + String(keyID));
        }
        else {
          client.publish(topicString,"error");
          printLog("Error añadiendo llavero en acceso " + nombre);
        }
      }
      client.publish(topicString,"memLlena");
        printLog("No es posible añadir llaveros, memoria llena en acceso " + nombre);
    }

    void limpiarLlaveros () {
      for (int i = 0; i <= 10; i++) {
        pass[i] = 0;
      }
    }

};
