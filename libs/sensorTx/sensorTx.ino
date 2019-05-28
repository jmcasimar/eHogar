#include <sensorTx.h>



sensorTx test(2738, "movimiento");

void setup() {
  Serial.begin(115200);
  test.begin(26, 15, HIGH);
  test.radioBegin();
}

void loop() {
  test.monitor();
}
