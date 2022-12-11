#include <HardwareSerial.h>

HardwareSerial MySerial(1);

int i = 0;
int b =0;
void setup() {
  Serial.begin(115200);
  
//  while (!Serial2 or !Serial1) {
//    Serial.print("Serial1: ");
//    Serial.print(Serial1);
//    Serial.print(" Serial2: ");
//    Serial.println(Serial2);
//    delay(100);
//  }

MySerial.begin(9600, SERIAL_8N1, 16, 17);

}

//------------------------------------------------------------------------------------
void loop() {
  
    while (MySerial.available() > 0) {
        uint8_t byteFromSerial = MySerial.read();
        Serial.print("Recieved: ");
        Serial.println(byteFromSerial);
    }

//Serial.println("Hello");

 delay(100);
  
}
 
