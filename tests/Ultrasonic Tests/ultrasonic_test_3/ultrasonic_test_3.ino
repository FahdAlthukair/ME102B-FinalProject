//Initialize Ultrasonic
#define ECHO_LEFT_PIN 34
#define TRIG_LEFT_PIN 26
#define ECHO_RIGHT_PIN 4
#define TRIG_RIGHT_PIN 25

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

//Sensor timers
unsigned long pingTimerRight; // Start now.
unsigned long pingTimerLeft; // Start now
unsigned int pingSpeed = 50;

float distanceCm_left, distanceCm_middle, distanceCm_right;

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(ECHO_LEFT_PIN, INPUT);
  pinMode(TRIG_LEFT_PIN, OUTPUT);


  pinMode(ECHO_RIGHT_PIN, INPUT);
  pinMode(TRIG_RIGHT_PIN, OUTPUT);


  //Sensor timers
  pingTimerRight = millis()+ 50; // Start now.
  pingTimerLeft = millis() + 150; // Start now
}

void loop() {

  if (millis() >= pingTimerLeft) {
    pingTimerLeft += pingSpeed;
    echoSensor("LEFT");
  }


  if (millis() >= pingTimerRight) {
    pingTimerRight += pingSpeed;
    echoSensor("RIGHT");
  }
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance left: ");
  Serial.println(distanceCm_left);
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
  }  else {
    distanceCm_right = distance;
  }
  

  
  
}
