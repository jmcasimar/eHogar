#include <autoCreator.h>

void autoCreation(String loc, String Type)
  {  new autoCreator(loc, Type); }

void setup() {
  Serial.begin(9600);
  Serial.print(F("Actualmente tienes : ")); Serial.print(autoCreator::__TotalObjects); Serial.println(F(" módulos creados"));
  autoCreation("Sala","HumSensor");
  Serial.print(F("Actualmente tienes : ")); Serial.print(autoCreator::__TotalObjects); Serial.println(F(" módulos creados"));
  autoCreation("Cocina","TempSensor");
  Serial.print(F("Actualmente tienes : ")); Serial.print(autoCreator::__TotalObjects); Serial.println(F(" módulos creados"));
  
  for (int i = 0; i < autoCreator::__TotalObjects; i++){
      Serial.print(F("El módulo ")); Serial.print(i); Serial.print(F(" se encuentra en: ")); Serial.println((autoCreator::ptr[i]->__Location));
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
