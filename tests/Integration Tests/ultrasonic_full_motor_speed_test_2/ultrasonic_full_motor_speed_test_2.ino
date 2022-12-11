//Initialize Ultrasonic
#define ECHO_PIN 34
#define TRIG_PIN 26

//Initialize Motors
#define ENA 13
#define IN1 12
#define IN2 27
#define IN3 33
#define IN4 15
#define ENB 32

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

//Sensor timers
unsigned long pingTimerUs;
unsigned int pingSpeed = 50;

float distanceCm;

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT); 


  //Sensor timers
  pingTimerUs = millis()+ 50; // Start now.
}

void loop() {

  if (millis() >= pingTimerUs) {
    pingTimerUs += pingSpeed;
    echoSensor();
  }

  analogWrite(ENA,255);
  analogWrite(ENB, 255);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distanceCm);
  
  delay(1000);
}

void echoSensor(){

  int trigPin = TRIG_PIN;
  int echoPin = ECHO_PIN;
  long duration;
  float distance;


  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distance = duration * SOUND_SPEED/2;
  distanceCm = distance;
}
