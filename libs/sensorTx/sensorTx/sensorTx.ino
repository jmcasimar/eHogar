#include <sensorTx.h>

boolean IS_RFM69HW = false; // If High power RFM69HW is used

#ifdef ESP32                // Allows automatic ESP32 processor recognition
#include <WiFi.h>          // Required for ESP32
#define X                  // eXtended Library Selector mandatory for ESP32
#endif
#include <RFM69X.h> //  eXteneded RFM69 base library

#define NODEID      99
#define NETWORKID   100
#define REMOTEID   1
bool SEND_PROMISCUOUS = false;       // Set to 'true' to sniff all packets on the same network

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

RFM69X radi(RFM_SS, RFM_INT, IS_RFM69HW, RFM_INTNUM);         // Create Extended RFM69radi instance
sensorTx test(2738, "movimiento");

void setup() {
  Serial.begin(115200);
  test.begin(26, 15, HIGH);
  test.radioBegin(radi, FREQUENCY, NODEID, REMOTEID, NETWORKID, IS_RFM69HW, SEND_PROMISCUOUS);
  radi.encrypt(ENCRYPTKEY);

}

void loop() {
  test.monitor(radi);
}
