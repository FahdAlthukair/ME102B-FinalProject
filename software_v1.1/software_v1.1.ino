
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

//Ultrasonic max distance
const float MAX_DISTANCE = 100.0;

//determine the normalization factor basen on MAX_DISTANCE
const float DISTANCE_FACTOR = MAX_DISTANCE / 100;
const float STOP_DISTANCE = 10;

// setting PWM properties
const int MAX_PWM_VOLTAGE = 255;
const int NOM_PWM_VOLTAGE = 150;
const int MIN_PWM_VOLTAGE = 60;

// determine the normalization factor based on MOTOR_BASE_SPEED
const float MOTOR_FACTOR_MAX = MAX_PWM_VOLTAGE / 100;
const float MOTOR_FACTOR_NOM = NOM_PWM_VOLTAGE / 100;

// motor compensation
const float L_MOTOR_FACTOR = 1.5;
const float R_MOTOR_FACTOR = 1.0;
const float L_MOTOR_FACTOR_THRESH = 80;
const float R_MOTOR_FACTOR_THRESH = 80;



// Ultrasonic timing
unsigned long pingTimerRight; 
unsigned long pingTimerMiddle; 
unsigned long pingTimerLeft; 
unsigned int pingSpeed = 50;

// Motor timing
unsigned long motorCm;
unsigned long motorPm;
const unsigned long MOTOR_PERIOD = 20;

// current US distance reading
float distance_left = 0;
float distance_middle = 0;
float distance_right = 0;

// past US distance reading
float distance_left_prev = 0;
float distance_right_prev = 0;

void setup() {
  
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

  Serial.begin(115200);
  
}

void loop() {

  //update the current distance
  usReadCm();

  // update the motor speeds
  setMotors();

  delay(100);
}

void usReadCm() {
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

  //if (distance == 0 or distance > MAX_DISTANCE) distance = MAX_DISTANCE;

  if (side == "LEFT") {
    distance_left = distance;
    Serial.print("Distance left: ");
    Serial.println(distance_left);
  } else if (side == "MIDDLE") {
    distance_middle = distance;
    Serial.print("Distance middle: ");
    Serial.println(distance_middle);
  } else {
    distance_right = distance;
    Serial.print("Distance right: ");
    Serial.println(distance_right);
  }

}

void setMotors() {
  motorCm = millis();
  if (motorCm > motorPm + MOTOR_PERIOD) {

    float motor_PWM_left =  MAX_PWM_VOLTAGE;
    float motor_PWM_right =  MAX_PWM_VOLTAGE;
    bool check_voltage = true;

    if (distance_middle <= MAX_DISTANCE) {

      float magnitude = (float)(MAX_DISTANCE - distance_middle) / DISTANCE_FACTOR;

     // motor_PWM_left = MAX_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR_MAX);
      //motor_PWM_right = motor_PWM_left;

      // To avoid going to walls/obstacles if no person in the middle
      distance_left_prev = distance_left;
      distance_right_prev = distance_right;
      
    } else if (distance_left <= MAX_DISTANCE and distance_left != distance_left_prev) {
      float magnitude = (float)(MAX_DISTANCE - distance_left) / DISTANCE_FACTOR;
//      motor_PWM_left = NOM_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR_NOM);
//      motor_PWM_right = MAX_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR_MAX);
      motor_PWM_left = NOM_PWM_VOLTAGE;
      motor_PWM_right = MAX_PWM_VOLTAGE;
    } else if (distance_right <= MAX_DISTANCE and distance_right != distance_right_prev) {
      float magnitude = (float)(MAX_DISTANCE - distance_right) / DISTANCE_FACTOR;
//      motor_PWM_right = NOM_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR_NOM);
//      motor_PWM_left = MAX_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR_MAX);
      motor_PWM_right = NOM_PWM_VOLTAGE;
      motor_PWM_left = MAX_PWM_VOLTAGE;
    } else {
      motor_PWM_left = 0;
      motor_PWM_right = 0;
      check_voltage = false;
    }

    //lower limit check
    if (motor_PWM_left < MIN_PWM_VOLTAGE and check_voltage) {
        motor_PWM_left = MIN_PWM_VOLTAGE;
      }
    if (motor_PWM_right < MIN_PWM_VOLTAGE and check_voltage) {
        motor_PWM_right = MIN_PWM_VOLTAGE;
      }

    // add in motor compensation
    if (motor_PWM_left <= L_MOTOR_FACTOR_THRESH) {
      motor_PWM_left *= L_MOTOR_FACTOR;
    }
    if (motor_PWM_right <= R_MOTOR_FACTOR_THRESH) {
      motor_PWM_right *= R_MOTOR_FACTOR;
    }
      
    if (distance_middle <= STOP_DISTANCE) {
      motor_PWM_left = 0;
      motor_PWM_right = 0;
    }

    analogWrite(ENA, motor_PWM_left);
    analogWrite(ENB, motor_PWM_right);
  
    // Turn on motor A & B
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    motorPm = motorCm;

    Serial.print("Left motor speed: ");
    Serial.println(motor_PWM_left);
    Serial.print("Right motor speed: ");
    Serial.println(motor_PWM_right);
  }
}
