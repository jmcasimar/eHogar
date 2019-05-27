#include <sensorTx.h>

boolean IS_RFM69HW = false; // If High power RFM69HW is used
//Match frequency to the hardware version of the radi on your Moteino (uncomment one):
//#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY   RF69_868MHZ
#define FREQUENCY     RF69_915MHZ
/***        ***/

#ifdef ESP32                // Allows automatic ESP32 processor recognition
#include <WiFi.h>          // Required for ESP32
#define X                  // eXtended Library Selector mandatory for ESP32
#endif

long lastPeriod = -1;

#include <RFM69X.h> //  eXteneded RFM69 base library  

#define NODEID      99
#define NETWORKID   100
#define REMOTEID   1
//Match frequency to the hardware version of the radi on your Moteino (uncomment one):
//#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY   RF69_868MHZ
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY   "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!

// Adapt configuration against the processor Type
#ifdef __AVR_ATmega1284P__
#define LED         15        // Moteino MEGA have LED on D15
#define RFM_SS      4         // Default Slave Select Moteino MEGA 
#define RFM_INT     2         // One if a free GPIO supporting Interrupts
#endif
#ifdef __AVR_ATmega328P__
#define LED         9         // Moteino has LEDs on D9
#define RFM_SS      10        // Default Slave Select for Arduino UNO or Moteino
#define RFM_INT     2         // One if a free GPIO supporting Interrupts
#endif
#ifdef ESP32
#define LED         2         // LED is on pin 2 for ESP-32          
#define RFM_SS      SS         // Default Slave Select PIN 5 for LOLIN 32
#define RFM_INT     4        // One if a free GPIO supporting Interrupts
#endif

#ifdef __AVR_ATmega1284P__
byte RFM_INTNUM = 2; // digitalPinToInterrupt doesn't work for ATmeg1284p
#else
byte RFM_INTNUM = digitalPinToInterrupt(RFM_INT);  // Standard way to convert Interrupt pin in interrupt number
#endif

int TRANSMITPERIOD = 500;        // Transmit a packet to gateway so often (in ms)
byte sendSize = 0;
long int frameCnt = 0;  // frame Counter
long int ackSentCnt = 0;  // ACK Counter


RFM69X radi(RFM_SS, RFM_INT, IS_RFM69HW, RFM_INTNUM);         // Create Extended RFM69radi instance

// RFM69 parameters
bool SEND_PROMISCUOUS = false;       // Set to 'true' to sniff all packets on the same network
byte SEND_RTRY = 0;                  // Adjust the attempts of a Send With Reply is to be performed in case of timeout
unsigned long SEND_WAIT_WDG = 40;    // Adjust the Send With Reply wait time for which a acknowledge is expected (default is 40ms)
// RFM69 Session Key parameters
bool SESSION_KEY = true;             // Set usage of session mode (or not)
bool SESSION_3ACKS = false;          // Set 3 acks at the end of a session transfer (or not)
unsigned long SESSION_WAIT_WDG = 10;  // Adjust wait time of data reception in session mode is expected (default is 40ms)
#ifdef X
unsigned long SESSION_RSP_DLY = 400; // Adjust the delay time between a Session Request and a Session reply for slow node should be set to at least 400 while using RFM69X_Session
#else
unsigned long SESSION_RSP_DLY = 200; // Adjust the delay time between a Session Request and a Session reply for slow node (default is 0us, maximum is 500us)
#endif

sensorTx test(2738, "movimiento");

void setup() {
  Serial.begin(115200);
  if (!radi.initialize(FREQUENCY, NODEID, NETWORKID))
  {
    Serial.println ("\n****************************************************************");
    Serial.println (" WARNING: RFM Transceiver intialization failure: Set-up Halted  ");
    Serial.println ("****************************************************************");
    while (1); // Halt the process
  }

  if (IS_RFM69HW)  radi.setHighPower();      // Only for RFM69HW!
  radi.encrypt(ENCRYPTKEY);                  // Set encryption
  radi.promiscuous(SEND_PROMISCUOUS);        // Set promiscuous mode
#ifdef RFM_SESSION
  radi.useSessionKey(SESSION_KEY);           // Set session mode
  radi.sessionWaitTime(SESSION_WAIT_WDG);    // Set the Session Wait Watchdog
  radi.useSession3Acks(SESSION_3ACKS);       // Set the Session 3 Acks option
  radi.sessionRespDelayTime(SESSION_RSP_DLY);// Set the slow node delay timer
#endif
  Serial.print ("\nBoard Type: ");
#ifdef __AVR_ATmega1284P__
  Serial.println ("__AVR_ATmega1284P__");
#endif
#ifdef __AVR_ATmega328P__
  Serial.println ("__AVR_ATmega328P__");
#endif
#ifdef ESP8266
  Serial.println ("ESP8266");
#endif
#ifdef ESP32
  Serial.println ("ESP32");
#endif
#ifdef __AVR_ATmega2560__
  Serial.println ("__AVR_ATmega2560__");
#endif

#ifdef RFM_SESSION
#ifdef X
  Serial.println ("Using RFM69X Extended Session mode");
#else
  Serial.println ("Using RFM69 Session mode");
#endif
#else
#ifdef X
  Serial.println ("Using RFM69 Extended mode");
#else
  Serial.println ("Using RFM69 Basic mode");
#endif
#endif


  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  test.begin(26, 15, HIGH);
  test.radioBegin(radi, ackSentCnt, TRANSMITPERIOD, lastPeriod, NODEID, frameCnt, REMOTEID, SEND_RTRY, SEND_WAIT_WDG);

}

void loop() {
  test.monitor(radi);
}
