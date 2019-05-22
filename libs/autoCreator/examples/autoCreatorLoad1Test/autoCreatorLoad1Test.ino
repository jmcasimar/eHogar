#include <autoCreator.h>
#include <EEPROM.h>

// Serial comunication
const byte CMDBUFFER_SIZE = 32;
String inputstring = "";
bool input_string_complete = false;
const char zero_char = char(48);
const char space_char = char(32);

void serialEvent() {
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;
  
  if(input_string_complete==true){
    if(inputstring.charAt(0)==zero_char){ // cleanEeprom--> '0'
      Serial.println(F("Borrando memoria EEPROM..."));
      autoCreator::cleanEeprom();
      Serial.println(F("Memoria EEPROM borrada"));
    }
    else if(inputstring.charAt(0)==zero_char+1){ // createNewObject--> '1'
      String command = inputstring.substring(1);
      String command1, command2, command3;
      int divisor = 0;
      for (int i = 0; i<command.length(); i++){
        if(command.charAt(i)==space_char && divisor==0){
          divisor = i;
        }
        else if(command.charAt(i)==space_char && divisor!=0){
          command1 = command.substring(0, divisor);
          command2 = command.substring(divisor+1, i);
          command3 = command.substring(i+1,command.length());
          autoCreator::autoCreation(command1, command2, command3, HIGH);
          break;
        }
      }
    }
  }

  input_string_complete = false;
}

void setup() {
  
  EEPROM.begin(1024);
  Serial.begin(115200);

  // Reserve 30 bytes for serial strings
  inputstring.reserve(30);
  
  autoCreator::loadObjects();

  Serial.print(F("Se han cargado ")); Serial.print(autoCreator::__TotalObjects); Serial.println(F(" módulos"));
  for (int i = 0; i < autoCreator::__TotalObjects; i++){
      Serial.print(F("El módulo ")); Serial.print(i); Serial.print(F(" se encuentra en ")); Serial.print((autoCreator::ptr[i]->__Location));
      Serial.print(F(" y es del tipo ")); Serial.print((autoCreator::ptr[i]->__Type)); Serial.print(F(" ID=")); Serial.println((autoCreator::ptr[i]->__ID));
  }

  
}

void loop() {

serialEvent();

}
