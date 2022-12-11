

//Initialize Ultrasonic
#define ECHO_LEFT_PIN 12
#define TRIG_LEFT_PIN 13
#define ECHO_MIDDLE_PIN 33
#define TRIG_MIDDLE_PIN 27
#define ECHO_RIGHT_PIN 32
#define TRIG_RIGHT_PIN 15

//Initialize Motors
#define ENA 14
#define IN1 22
#define IN2 23
#define IN3 36
#define IN4 39
#define ENB 34


//Ultrasonic max distance
const float MAX_DISTANCE = 100.0;

//determine the normalization factor basen on MAX_DISTANCE
const float DISTANCE_FACTOR = MAX_DISTANCE / 100;
const float STOP_DISTANCE = 5;

// setting PWM properties
const int MAX_PWM_VOLTAGE = 255;
const int NOM_PWM_VOLTAGE = 150;
const int MIN_PWM_VOLTAGE = 20;

// determine the normalization factor based on MOTOR_BASE_SPEED
const float MOTOR_FACTOR = MAX_PWM_VOLTAGE / 100;

// Ultrasonic timing
unsigned long usCm;
unsigned long usPm;
const unsigned long US_PERIOD = 50;

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
  
  Serial.begin(115200);
}

void loop() {

  //update the current distance
  usReadCm();

  // update the motor speeds
  setMotors();
}

void usReadCm() {
  usCm = millis();
  if (usCm > usPm + US_PERIOD) {
    digitalWrite(TRIG_LEFT_PIN, LOW);
    digitalWrite(TRIG_MIDDLE_PIN, LOW);
    digitalWrite(TRIG_RIGHT_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_LEFT_PIN, HIGH);
    digitalWrite(TRIG_MIDDLE_PIN, HIGH);
    digitalWrite(TRIG_RIGHT_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_LEFT_PIN, LOW);
    digitalWrite(TRIG_MIDDLE_PIN, LOW);
    digitalWrite(TRIG_RIGHT_PIN, LOW);

    long duration_left = pulseIn(ECHO_LEFT_PIN, HIGH, 38000);
    long duration_middle = pulseIn(ECHO_MIDDLE_PIN, HIGH, 38000);
    long duration_right = pulseIn(ECHO_RIGHT_PIN, HIGH, 38000);

    distance_left_prev = distance_left;
    distance_right_prev = distance_right;


    distance_left = duration_left * 0.034/2;
    distance_middle = duration_middle * 0.034/2;
    distance_right = duration_right * 0.034/2;
    

    if (distance_left == 0 or distance_left > MAX_DISTANCE) distance_left = MAX_DISTANCE;
    if (distance_middle == 0 or distance_middle > MAX_DISTANCE) distance_middle = MAX_DISTANCE;
    if (distance_right == 0 or distance_right > MAX_DISTANCE) distance_right = MAX_DISTANCE;

    usPm = usCm;

    Serial.print("Distance left: ");
    Serial.print(distance_left);
    Serial.print(" Distance middle: ");
    Serial.print(distance_middle);
    Serial.print(" Distance right: ");
    Serial.print(distance_right);
  }
}

void setMotors() {
  motorCm = millis();
  if (motorCm > motorPm + MOTOR_PERIOD) {

    float motor_PWM_left =  MAX_PWM_VOLTAGE;
    float motor_PWM_right =  MAX_PWM_VOLTAGE;
    float left_change = distance_left - distance_left_prev;
    left_change = abs(left_change);
    float right_change = distance_right - distance_right_prev;
    right_change = abs(right_change);

    if (distance_middle <= MAX_DISTANCE) {

      float magnitude = (float)(MAX_DISTANCE - distance_middle) / DISTANCE_FACTOR;

      motor_PWM_left = MAX_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR);
      motor_PWM_right = motor_PWM_left;

      if (motor_PWM_left < MIN_PWM_VOLTAGE) {
        motor_PWM_left = MIN_PWM_VOLTAGE;
        motor_PWM_right = motor_PWM_left;
      }
      
    } else if (distance_left <= MAX_DISTANCE and left_change > 5) {
      motor_PWM_left = NOM_PWM_VOLTAGE;
      motor_PWM_right = MAX_PWM_VOLTAGE;
    } else if (distance_right <= MAX_DISTANCE and right_change > 5) {
      motor_PWM_right = NOM_PWM_VOLTAGE;
      motor_PWM_left = MAX_PWM_VOLTAGE;
    } else {
      motor_PWM_left = 0;
      motor_PWM_right = 0;
    }


    if (distance_middle <= STOP_DISTANCE) {
      motor_PWM_left = 0;
      motor_PWM_right = 0;
    }

    analogWrite(ENA, motor_PWM_left);
    analogWrite(ENB, motor_PWM_right);
  
    // Turn on motor A & B
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    motorPm = motorCm;
  }
}
