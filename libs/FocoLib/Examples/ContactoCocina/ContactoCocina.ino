// Incluir librerías
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <FocoLib.h>

// Definir propiedades de red
const char* host = "esp32";
char _ssid[16];
char _password[16];
const char* mqtt_server = "192.168.0.18";

// Crear objetos y variables de red
WiFiClient espClient;
PubSubClient client(espClient);
int conecTimmer = 0;
long lastMsg = 0;
char msg[50];
int value = 0;
const int recon = 300; // Tiempo de intento de reconexión en segundos
String mac;
char clientID[20];

// Definir propiedades de PWM
const int freq = 5000;
const int resolution = 8;

// Definir variables para enviar log
char logTopic[20] = "log/cocina";
char logMsg[100];
String _logMsg;

void printLog(String msg) {
      msg.toCharArray(logMsg, 50);
      Serial.println(logMsg);
      client.publish(logTopic, logMsg);
}

// Construir clases
class RGBled {

  private:
    int _pinR;
    int _pinG;
    int _pinB;
    int _idR;
    int _idG;
    int _idB;
    int ID;

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
      ledcSetup(_idR, freq, resolution);
      ledcSetup(_idG, freq, resolution);
      ledcSetup(_idB, freq, resolution);
      ledcAttachPin(_pinR, _idR);
      ledcAttachPin(_pinG, _idG);
      ledcAttachPin(_pinB, _idB);
    }

    void set(int r, int g, int b) {
      ledcWrite(_idR, r);
      ledcWrite(_idG, g);
      ledcWrite(_idB, b);
      printLog("Valores LED: R: " + String(r) + " G: " + String(g) + " B: " + String(b));
    }
};

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

// Crear objetos
Proximidad proximidadCocina(1, 21, "Cocina");
Proximidad proximidadPasillo(1, 22, "Pasillo");
RGBled rgbLed1(1, 32, 33, 25);
Foco focoCocina(1, 19, "iluminacion/cocina/output", "iluminacion/cocina/log", "Cocina");
Foco focoPasillo(1, 20, "iluminacion/pasillo/output", "iluminacion/pasillo/log", "Pasillo");
WebServer server(80);

// Crear servidor web
/*
   Login page
*/

const char* loginIndex =
  "<head>"
  "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/>"
  "</head>"
  "<form name='loginForm'>"
  "<table width='20%' bgcolor='DADAD9' align='center'>"
  "<tr>"
  "<td colspan=2>"
  "<center><font size=4><b>Inicio de sesión</b></font></center>"
  "<br>"
  "</td>"
  "<br>"
  "<br>"
  "</tr>"
  "<td>Usuario:</td>"
  "<td><input type='text' size=25 name='userid'><br></td>"
  "</tr>"
  "<br>"
  "<br>"
  "<tr>"
  "<td>Contraseña:</td>"
  "<td><input type='Password' size=25 name='pwd'><br></td>"
  "<br>"
  "<br>"
  "</tr>"
  "<tr>"
  "<td><input type='submit' onclick='check(this.form)' value='Iniciar sesión'></td>"
  "</tr>"
  "</table>"
  "</form>"
  "<script>"
  "function check(form)"
  "{"
  "if(form.userid.value=='admin' && form.pwd.value=='admin')"
  "{"
  "window.open('/menuIndex')"
  "}"
  "else"
  "{"
  " alert('Error en la autenticación')/*displays error message*/"
  "}"
  "}"
  "</script>";

/*
   Server Index Page
*/

const char* menuIndex =
  "<form name='menu'>"
  "<button onclick='firm(this.form)'>Actualizar firmware</button>"
  "<button onclick='wifi(this.form)'>Actualizar WiFi</button>"
  "<button onclick='terminal(this.form)'>Abrir terminal</button>"
  "</form>"
  "<script>"
  "function firm(form)"
  "{"
  "window.open('/serverIndex')"
  "}"
  "</script>"
  "<script>"
  "function wifi(form)"
  "{"
  "window.open('/wifiIndex')"
  "}"
  "</script>"
  "<script>"
  "function terminal(form)"
  "{"
  "window.open('/terminalIndex')"
  "}"
  "</script>";

const char* wifiIndex =
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
  "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Actualizar'>"
  "</form>"
  "<div id='prg'>Avance: 0%</div>"
  "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('Avance: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('Completado')"
  "},"
  "error: function (a, b, c) {"
  "}"
  "});"
  "});"
  "</script>";

const char* terminalIndex =
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
  "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Actualizar'>"
  "</form>"
  "<div id='prg'>Avance: 0%</div>"
  "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('Avance: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('Completado')"
  "},"
  "error: function (a, b, c) {"
  "}"
  "});"
  "});"
  "</script>";

const char* serverIndex =
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
  "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Actualizar'>"
  "</form>"
  "<div id='prg'>Avance: 0%</div>"
  "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('Avance: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('Completado')"
  "},"
  "error: function (a, b, c) {"
  "}"
  "});"
  "});"
  "</script>";

void webServer() {
  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    printLog("Error estableciendo el servidor mDNS");
    while (1) {
      delay(1000);
    }
  }
  printLog("mDNS inicializado");
 
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/menuIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", menuIndex);
  });
  server.on("/wifiIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", wifiIndex);
  });
  server.on("/terminalIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", terminalIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      printLog("Actualizar con: " + String(upload.filename));
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        printLog("Actualización completada: %u\nReiniciando...\n" + String(upload.totalSize));
      } else {
        Update.printError(Serial);
      }
    }
  });
}

// Crear funciones de conexión
void setup_MQTT() {
  rgbLed1.set(100, 50, 50);
  setup_wifi();
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.print("Probando conexión MQTT... ");
      if (client.connect(clientID)) {
        rgbLed1.set(50, 50, 250);
        printLog("MQTT conectado");
        // Subscribirse
        client.subscribe("iluminacion/cocina/input");
        client.subscribe("iluminacion/pasillo/input");
        client.subscribe("status/cocina");
        IPAddress ip;
        ip = (WiFi.localIP());
        String ipS = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        char ipC[20];
        ipS.toCharArray(ipC, 20);
        client.publish("ipLog/cocina", ipC);
      } else {
        Serial.print("error, rc=");
        Serial.println(client.state());
        Serial.println("MQTT no conectado");
        rgbLed1.set(255, 0, 5);
      }
    }
  }
}
void setup_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    int retry = 0;
    delay(10);
    Serial.println();
    Serial.print("Conectando a ");
    Serial.print(_ssid);

    WiFi.begin(_ssid, _password);

    while (WiFi.status() != WL_CONNECTED && retry < 15) {
      delay(500);
      Serial.print(".");
      retry++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.print( "WiFi conectado, la dirección IP es ");
      Serial.println(WiFi.localIP());
    }
    else {
      Serial.println("WiFi no conectado");
    }
  }
}


// Crear callback para servidor MQTT
void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;

  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

   printLog("Mensaje recibido en topic: " + String(topic) + ". Mensaje: " + messageTemp);

  int Value = messageTemp.toInt();

  if (String(topic) == "iluminacion/cocina/input") {
    focoCocina.setState(Value);
  }
  if (String(topic) == "iluminacion/pasillo/input") {
    focoPasillo.setState(Value);
  }

  if (String(topic) == "status/cocina" && messageTemp == "envia")
  {
    client.publish("status/cocina", "conectado");
  }
}

void setup() {
  Serial.begin(115200); // Inicializar puerto serial
  EEPROM.begin(64); // Inicializar 64 kb de memoria flash

  //Inicializar sensores
  focoCocina.setup(freq, resolution, client);
  focoPasillo.setup(freq, resolution, client);

  // Asignar o cargar configuración wifi
  String ssid = EEPROM.readString(0);
  ssid = "b504e8";
  //EEPROM.writeString(0,ssid);
  ssid.toCharArray(_ssid, sizeof(_ssid));

  String password = EEPROM.readString(16);
  password = "278664203";
  //EEPROM.writeString(16,password);
  password.toCharArray(_password, sizeof(_password));
  //EEPROM.commit();

  // Configurar IP fija
  IPAddress local_IP(192, 168, 0, 20);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 0, 0);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Falló la configuración de la IP fija");
  }

  // Obtener dirección MAC
  WiFi.mode(WIFI_MODE_STA);
  mac = WiFi.macAddress();
  mac.replace(":", "");
  mac.toCharArray(clientID, 20);
  Serial.println("El clientID es " + String(clientID));
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  setup_MQTT(); //Inicializar WiFi y servidor MQTT

  // Crear e inicializar servidor web
  webServer();
  server.begin();
}

void loop() {
  // Ejecutar cliente MQTT o reconectar si está desconectado
  if (client.connected()) {
    client.loop();
  }
  else {
    int now = millis() / 1000;
    if (now - conecTimmer > recon)
    {
      setup_MQTT();
      conecTimmer = now;
    }
  }

  // Ejecutar cleinte de servidor web
  server.handleClient();

  //Revisar entradas para focos
  //Foco del pasillo
  if (proximidadPasillo.read()) {
    focoPasillo.setState(!focoPasillo.getState()*100);
  }

  // Foco de la cocina
  if (proximidadCocina.read()) {
    focoCocina.setState(!focoCocina.getState()*100);
  }

  // Regular focos
  focoCocina.dimm(client);
  focoPasillo.dimm(client);
}
