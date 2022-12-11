//Initialize Motors
#define IN3 33
#define IN4 15
#define ENB 32

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  
 analogWrite(ENB,255);
 digitalWrite(IN3,HIGH);
 digitalWrite(IN4,LOW);
 delay(2000);
 analogWrite(ENB,150);
 delay(2000);
// analogWrite(ENB,255);
// digitalWrite(IN4,HIGH);
// digitalWrite(IN3,LOW);
// delay(2000);
// analogWrite(ENB,150);
// delay(2000);
}
