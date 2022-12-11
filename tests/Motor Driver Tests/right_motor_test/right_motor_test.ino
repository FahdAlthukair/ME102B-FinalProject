//Initialize Motors
#define ENA 13
#define IN1 12
#define IN2 27

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void loop() {
  
 analogWrite(ENA,180);
 digitalWrite(IN1,HIGH);
 digitalWrite(IN2,LOW);
 delay(2000);
// analogWrite(ENA,200);
// delay(2000);
// analogWrite(ENA,255);
// digitalWrite(IN2,HIGH);
// digitalWrite(IN1,LOW);
// delay(2000);
// analogWrite(ENA,150);
// delay(2000);
}
