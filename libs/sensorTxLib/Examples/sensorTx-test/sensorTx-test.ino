#include <sensorTx.h>

sensorTx test(2738, "movimiento"); 

void setup() {
  Serial.begin(115200);
  test.begin(23, 22, 21, HIGH);
}

void loop() {
  test.monitor();
  delay(50);
}
