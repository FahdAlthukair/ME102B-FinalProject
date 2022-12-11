//Initialize Ultrasonic
#define ECHO_LEFT_PIN 34
#define TRIG_LEFT_PIN 26
#define ECHO_MIDDLE_PIN 39
#define TRIG_MIDDLE_PIN 25
#define ECHO_RIGHT_PIN 36
#define TRIG_RIGHT_PIN 4

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
unsigned long pingTimerRight; // Start now.
unsigned long pingTimerMiddle; // Start now.
unsigned long pingTimerLeft; // Start now
unsigned int pingSpeed = 50;

float distanceCm_left, distanceCm_middle, distanceCm_right;

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(ECHO_LEFT_PIN, INPUT);
  pinMode(TRIG_LEFT_PIN, OUTPUT);
  pinMode(ECHO_MIDDLE_PIN, INPUT);
  pinMode(TRIG_MIDDLE_PIN, OUTPUT);
  pinMode(ECHO_RIGHT_PIN, INPUT);
  pinMode(TRIG_RIGHT_PIN, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT); 


  //Sensor timers
  pingTimerRight = millis()+ 50; // Start now.
  pingTimerMiddle = millis() + 100; // Start now.
  pingTimerLeft = millis() + 150; // Start now
}

void loop() {

  if (millis() >= pingTimerLeft) {
    pingTimerLeft += pingSpeed;
    echoSensor("LEFT");
  }

  if (millis() >= pingTimerMiddle) {
    pingTimerMiddle += pingSpeed;
    echoSensor("MIDDLE");
  }

  if (millis() >= pingTimerRight) {
    pingTimerRight += pingSpeed;
    echoSensor("RIGHT");
  }

  analogWrite(ENA,255);
  analogWrite(ENB, 255);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance left: ");
  Serial.println(distanceCm_left);
  Serial.print("Distance middle: ");
  Serial.println(distanceCm_middle);
  Serial.print("Distance right: ");
  Serial.println(distanceCm_right);
  
  delay(1000);
}

void echoSensor(String side){

  int trigPin = TRIG_RIGHT_PIN;
  int echoPin = ECHO_RIGHT_PIN;
  long duration;
  float distance;

  if (side == "LEFT") {
    trigPin = TRIG_LEFT_PIN;
    echoPin = ECHO_LEFT_PIN;
  } else if (side == "MIDDLE") {
    trigPin = TRIG_MIDDLE_PIN;
    echoPin = ECHO_MIDDLE_PIN;
  } 

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

  if (side == "LEFT") {
    distanceCm_left = distance;
  } else if (side == "MIDDLE") {
    distanceCm_middle = distance;
  } else {
    distanceCm_right = distance;
  }
  

  
  
}
