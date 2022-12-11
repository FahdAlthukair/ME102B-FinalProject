#define sensor 25

void setup() {
  // put your setup code here, to run once:
  pinMode(sensor, INPUT);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  float distance = 0;
  for (int i = 0; i < 25; i++) {
    distance += analogRead(sensor);
  }
  distance /= 25;
//  float volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
//  int distance = 13*pow(volts, -1); // worked out from datasheet graph
  Serial.println(distance);
  delay(100); // slow down serial port 

}
