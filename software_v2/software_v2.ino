#include <HardwareSerial.h>

//Thermal camera attributes
HardwareSerial MySerial(1);
uint8_t arrayCenter = 40;

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

//Ultrasonic max distance
const float MAX_DISTANCE = 100.0;

//determine the normalization factor based on MAX_DISTANCE
const float DISTANCE_FACTOR = MAX_DISTANCE / 100;

int stop_distance = 10;
const int STOP_DISTANCE_1 = 10;
const int STOP_DISTANCE_2 = 15;

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
unsigned long pingTimerUs; 
unsigned int pingSpeed = 50;

// Motor timing
unsigned long motorCm;
unsigned long motorPm;
const unsigned long MOTOR_PERIOD = 20;

// current US distance reading
float distance = 0;

// past US distance reading
float distance_prev = 0;

void setup() {
  
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

  Serial.begin(115200);
  MySerial.begin(9600, SERIAL_8N1, 16, 17);
  
}

void loop() {

  //update array center from teensy
  thermalRead();
  
  //update the current distance
  usReadCm();

  // update the motor speeds
  setMotors();

  delay(100);
}

void thermalRead() {
  if (MySerial.available() > 0) {
     arrayCenter = MySerial.read();
     Serial.println();
     Serial.print("Array center is: ");
     Serial.println(arrayCenter);
     Serial.println();
  }
}

void usReadCm() {
  if (millis() >= pingTimerUs) {
    pingTimerUs += pingSpeed;
    echoSensor();
  }
}

void echoSensor(){

  int trigPin = TRIG_PIN;
  int echoPin = ECHO_PIN;
  long duration;

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
  Serial.print("Distance is: ");
  Serial.println(distance);

}

void setMotors() {
  motorCm = millis();
  if (motorCm > motorPm + MOTOR_PERIOD) {

      float motor_PWM_left =  0;
      float motor_PWM_right =  0;
      bool check_voltage = true;
    if (arrayCenter >= 0 && arrayCenter <= 33) {
      
      //target on the right side
      if (arrayCenter > 0 && arrayCenter < 14) {
//      motor_PWM_right = NOM_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR_NOM);
//      motor_PWM_left = MAX_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR_MAX);
        motor_PWM_right = NOM_PWM_VOLTAGE;
        motor_PWM_left = MAX_PWM_VOLTAGE;
        
      } else if (arrayCenter > 18 && arrayCenter < 31) {
//      motor_PWM_left = NOM_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR_NOM);
//      motor_PWM_right = MAX_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR_MAX);
        motor_PWM_left = NOM_PWM_VOLTAGE;
        motor_PWM_right = MAX_PWM_VOLTAGE;
      } else if (distance <= MAX_DISTANCE) {
        
          if (distance < stop_distance) {
            motor_PWM_left = 0;
            motor_PWM_right = 0;
            stop_distance = STOP_DISTANCE_2;
            check_voltage = false;
          } else {
         //float magnitude = (float)(MAX_DISTANCE - distance) / DISTANCE_FACTOR;
         // motor_PWM_left = MAX_PWM_VOLTAGE - (magnitude * MOTOR_FACTOR_MAX);
         //motor_PWM_right = motor_PWM_left;
         motor_PWM_left = MAX_PWM_VOLTAGE;
         motor_PWM_right = MAX_PWM_VOLTAGE;
         distance_prev = distance;
         stop_distance = STOP_DISTANCE_1;
          }
      } else {
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
    }
      

    analogWrite(ENA, motor_PWM_left);
    analogWrite(ENB, motor_PWM_right);
  
    // Turn on motor A & B
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    motorPm = motorCm;

//    Serial.print("Left motor speed: ");
//    Serial.println(motor_PWM_left);
//    Serial.print("Right motor speed: ");
//    Serial.println(motor_PWM_right);
  }
}
