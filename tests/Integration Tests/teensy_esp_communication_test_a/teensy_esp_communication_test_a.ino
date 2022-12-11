int i = 0;
void setup() {
Serial.begin(9600);

delay(3000);

  while (!Serial1) {
    Serial.println("Waiting for Serial1");
    delay(1);
  }

Serial.println("Found Serial1");
Serial1.begin(9600);

}

//------------------------------------------------------------------------------------
void loop() {

 if (i % 3 == 0) {
  Serial.println(" Sending 40");
  Serial1.write(40);
 }
 if (i % 4 == 0) {
  Serial.println(" Sending 20");
  Serial1.write(20);
 }
 i+=1;

 delay(100);
  
}
 
