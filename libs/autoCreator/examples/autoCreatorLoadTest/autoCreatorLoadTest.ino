#include <autoCreator.h>
#include <EEPROM.h>

void setup() {
  
  EEPROM.begin(1024);
  Serial.begin(115200);

  for (int i = 0; i < autoCreator::__TotalObjects; i++){
      Serial.print(F("El módulo ")); Serial.print(i); Serial.print(F(" se encuentra en: ")); Serial.println((autoCreator::ptr[i]->__Location));
 }
}

void loop() {
  // put your main code here, to run repeatedly:

}
