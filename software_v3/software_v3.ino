#include <Arduino.h>
#include <HardwareSerial.h>

//Thermal camera attributes
HardwareSerial MySerial(1);
uint8_t arrayCenter = 40;

//Initialize Ultrasonic
#define ECHO_PIN_RIGHT 34
#define TRIG_PIN_RIGHT 26
#define ECHO_PIN_LEFT 4
#define TRIG_PIN_LEFT 25

//Initialize Motors
#define ENA 13
#define IN1 12
#define IN2 27
#define IN3 33
#define IN4 15
#define ENB 32

// setting PWM properties
const int MOTOR_PWM_VOLTAGE = 170;
const int TURN_PWM_VOLTAGE = 200;


//define sound speed in cm/uS
#define SOUND_SPEED 0.034

//Ultrasonic max distance
const float MAX_DISTANCE = 300.0;

//determine the normalization factor based on MAX_DISTANCE
const float DISTANCE_FACTOR = MAX_DISTANCE / 100;

int stop_distance = 5;
const int STOP_DISTANCE_1 = 5;
const int STOP_DISTANCE_2 = 10;

// Ultrasonic timing
unsigned long pingTimerRight; 
unsigned long pingTimerLeft;
unsigned int pingSpeed = 80;

// current US distance reading
float distance_right = 0;
float distance_left = 0;

// current US distance reading
float prev_distance_right = 0;
float prev_distance_left = 0;

// Motor timing
unsigned long motorCm;
unsigned long motorPm;
const unsigned long MOTOR_PERIOD = 100;
//
//// Thermal timing
//unsigned long thermalTimer;
//const unsigned long THERMAL_PERIOD = 20;

//Motor btn setup
#define BTN 39  // declare the button pin number
bool motorsOn = true;

volatile bool buttonIsPressed = false;
volatile bool timePassed = true;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR isr() {  // the function to be called when interrupt is triggered
  portENTER_CRITICAL_ISR(&timerMux);
  buttonIsPressed = true; 
  timePassed = false;
  portEXIT_CRITICAL_ISR(&timerMux);
  timerStart(timer);

}

void IRAM_ATTR timer_isr() {  // the function to be called when interrupt is triggered
  portENTER_CRITICAL(&timerMux);
  timePassed = true; 
  portEXIT_CRITICAL(&timerMux);
  timerStop(timer);
  timerRestart(timer);
}

//Set currentState variable
int prevState = 0;
int currentState = 0;

void setup() {
  Serial.begin(115200);
  MySerial.begin(9600, SERIAL_8N1, 16, 17);

  pinMode(ECHO_PIN_RIGHT, INPUT);
  pinMode(TRIG_PIN_RIGHT, OUTPUT);
  pinMode(ECHO_PIN_LEFT, INPUT);
  pinMode(TRIG_PIN_LEFT, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  pinMode(BTN, INPUT); 
  attachInterrupt(BTN, isr, RISING);
  timer = timerBegin(0, 80, true); // divides the frequency by the prescaler: 80,000,000 / 80 = 1,000,000 tics / sec
  timerAttachInterrupt(timer, &timer_isr, true);    // sets which function do you want to call when the interrupt is triggered
  timerAlarmWrite(timer, 200000, true);         // sets how many tics will you count to trigger the interrupt
  timerAlarmEnable(timer); // Enables timer
  


  //US Sensor timers
  pingTimerRight = millis()+ 50; // Start now.
  pingTimerLeft = millis()+ 100; // Start now.

//  //Thermal Timer
//  thermalTimer = millis() + 50;
  
}

void loop() {
//  Serial.print("current state: ");
//  Serial.println(currentState);
  switch (currentState) {
    
    //Idling state. Waiting to see human or to get a button press
    case 0:
      //Check to see if the lid button is pressed to open
      if (checkMotorButton()) {
        //Move to motor closing state
        buttonIsPressed = false;
        prevState = 0;
        currentState = 4;
      } 
      //Check to see if human is in the center with enough distance to follow
      else if (checkHumanCenter()) {
        //Move to following state
        prevState = 0;
        currentState = 1;
        startMotors();
      }
      //Check to see if human is on the right side
      else if (checkHumanRight()) {
        //Move to the turning right state
        prevState = 0;
        currentState = 2;
        startMotors();
      }
      //Check to see if human is on the left side
      else if (checkHumanLeft()) {
        //Move to turning left state
        prevState = 0;
        currentState = 3;
        startMotors();
      }
      prevState = 0;
      break;
    //Following state. Move forward to follow human
    case 1:
      //Check to see if the lid button is pressed to open
      if (checkMotorButton()) {
        //Move to lid opening state
        buttonIsPressed = false;
        prevState = 1;
        currentState = 4;
      } 
      //Check to see if human is on the right side
      else if (checkHumanRight()) {
        //Move to the turning right state
        prevState = 1;
        currentState = 2;
      }
      //Check to see if human is on the left side
      else if (checkHumanLeft()) {
        //Move to turning left state
        prevState = 1;
        currentState = 3;
      }
      //Check to see if human is no longer at the center or not at an accepted distance
      else if (checkHumanCenter() == false) {
        //Stop motors and move to idling state
        prevState = 1;
        currentState = 0;
        stopMotors();
      } 
      else {
        //If none of the previous event checkers were triggered move forward
        moveForward();
      }
      prevState = 1;
      break;
    //Turning right state. Turn right until human is in the middle
    case 2:
      //Check to see if the lid button is pressed to open
      if (checkMotorButton()) {
        //Move to lid opening state
        buttonIsPressed = false;
        prevState = 2;
        currentState = 4;
      } 
      //Check to see if human is in the center with enough distance to follow
      else if (checkHumanCenter()) {
        //Move to following state
        prevState = 2;
        currentState = 1;
      }
      //Check to see if human is on the left side
      else if (checkHumanLeft()) {
        //Move to turning left state
        prevState = 2;
        currentState = 3;
      }
      //Check to see if human is no longer at the right after checking if human is in the center
      else if (checkHumanRight() == false) {
        //Stop motors and move to the idling state
        prevState = 2;
        currentState = 0;
        stopMotors();
      } else {
        //If none of the previous event checkers were triggered turn right
        turnRight();
      }
      prevState = 2;
      break;
    //Turning left state. Turn left until human is in the middle
    case 3:
      //Check to see if the lid button is pressed to open
      if (checkMotorButton()) {
        //Move to lid opening state
        buttonIsPressed = false;
        prevState = 3;
        currentState = 4;
      } 
      //Check to see if human is in the center with enough distance to follow
      else if (checkHumanCenter()) {
        //Move to following state
        prevState = 3;
        currentState = 1;
      }
      //Check to see if human is on the right side
      else if (checkHumanRight()) {
        //Move to the turning right state
        prevState = 3;
        currentState = 2;
      }
      //Check to see if human is no longer at the left after checking if human is in the center
      else if (checkHumanLeft() == false) {
        //Stop motors and move to idling state.
        prevState = 3;
        currentState = 0;
        stopMotors();
      } else {
        //If none of the previous event checkers were triggered turn left
        turnLeft();
      }
      prevState = 3;
      break;
    //Stop Motor State. Stop the motors until the button is pressed again. 
    case 4:
      //Check to see if motors are on
      if (motorsOn) {
        //Stop motors
        stopMotors();
        motorsOn = false;
      }
      //Check to see if the motor button is pressed to start motors
      if (checkMotorButton()) {
        //Start motors and go back to idling state
        prevState = 4;
        currentState = 0;
        buttonIsPressed = false;
        startMotors();
        motorsOn = true;
      } 
      prevState = 4;
      break; 
    default:
      Serial.println("Robot Error");
      break; 
  }

  
  delay(150);
  

}

bool checkMotorButton() {
  if (buttonIsPressed && timePassed) {
    Serial.println("Button is pressed");
    return true;
  }
  return false;
}

bool checkHumanCenter() {
    //Read from thermal camera
    thermalRead();
    if (arrayCenter >= 10 && arrayCenter <= 18) {
      usReadCm();
      if (distance_left > stop_distance && distance_right > stop_distance) {
        stop_distance = STOP_DISTANCE_1;
        return true;
      }
      else {
        //Serial.println("Returned false due to stopping distance");
        stop_distance = STOP_DISTANCE_2;
        return false;
      }
    }
    return false;
}

bool checkHumanRight() {
    thermalRead();
    if (arrayCenter > 0 && arrayCenter < 10) {
       usReadCm();
      if (distance_left > stop_distance && distance_right > stop_distance) {
        stop_distance = STOP_DISTANCE_1;
        return true;
      }
      else {
        //Serial.println("Returned false due to stopping distance");
        stop_distance = STOP_DISTANCE_2;
        return false;
      }
    }
    return false;
}

bool checkHumanLeft() {
    thermalRead();
    if (arrayCenter > 18 && arrayCenter <= 31) {
      usReadCm();
      if (distance_left > stop_distance && distance_right > stop_distance) {
        stop_distance = STOP_DISTANCE_1;
        return true;
      }
      else {
        //Serial.println("Returned false due to stopping distance");
        stop_distance = STOP_DISTANCE_2;
        return false;
      }
    }
    return false;
}

void stopMotors() {
  motorCm = millis();
  if (currentState != prevState && motorCm > motorPm + MOTOR_PERIOD) {
    Serial.println("Stopping Motors");
   digitalWrite(IN1,LOW);
   digitalWrite(IN2,LOW);
   digitalWrite(IN3,LOW);
   digitalWrite(IN4,LOW);
   motorPm = motorCm; 
  }
  
}

void moveForward() {
  motorCm = millis();
  if (currentState != prevState && motorCm > motorPm + MOTOR_PERIOD) {
    Serial.println("Moving Forward");
     analogWrite(ENA, MOTOR_PWM_VOLTAGE);
     analogWrite(ENB, MOTOR_PWM_VOLTAGE);
     motorPm = motorCm;
  }
}

void turnRight() {
  motorCm = millis();
  if (currentState != prevState && motorCm > motorPm + MOTOR_PERIOD) {
    Serial.println("Turning Right");
//    analogWrite(ENA, MOTOR_PWM_VOLTAGE);
//    analogWrite(ENB, TURN_PWM_VOLTAGE);
    analogWrite(ENA, 0);
    analogWrite(ENB, TURN_PWM_VOLTAGE);
    motorPm = motorCm;
  }
}

void turnLeft() {
  motorCm = millis();
  if (currentState != prevState && motorCm > motorPm + MOTOR_PERIOD) {
    Serial.println("Turning left");
//    analogWrite(ENA, TURN_PWM_VOLTAGE);
//    analogWrite(ENB, MOTOR_PWM_VOLTAGE);
    analogWrite(ENA, TURN_PWM_VOLTAGE);
    analogWrite(ENB, 0);
    motorPm = motorCm;
  }
}

void startMotors() {
  motorCm = millis();
  if (currentState != prevState && motorCm > motorPm + MOTOR_PERIOD) {
    Serial.println("Starting Motors");
     digitalWrite(IN1,HIGH);
     digitalWrite(IN2,LOW);
     digitalWrite(IN3,HIGH);
     digitalWrite(IN4,LOW); 
     motorPm = motorCm;
  }
}

void thermalRead() {
  if (MySerial.available() > 0) {
     arrayCenter = MySerial.read();
//     thermalTimer += THERMAL_PERIOD;
     Serial.print("Array center is: ");
     Serial.println(arrayCenter);
//     Serial.println();
//     Serial.print("Array center is: ");
//     Serial.println(arrayCenter);
//     Serial.println();
  }
}

void usReadCm() {
  if (millis() >= pingTimerRight) {
    pingTimerRight += pingSpeed;
    prev_distance_right = distance_right;
    echoSensor("RIGHT");
    if (distance_right > MAX_DISTANCE) {
      distance_right = prev_distance_right;
    }
  }

  if (millis() >= pingTimerLeft) {
    pingTimerLeft += pingSpeed;
    prev_distance_left = distance_left;
    echoSensor("LEFT");
    if (distance_left > MAX_DISTANCE) {
      distance_left = prev_distance_left;
    }
//    Serial.print("Distance is: ");
//    Serial.println(distance);
  }
}

void echoSensor(String side){

  int trigPin = TRIG_PIN_LEFT;
  int echoPin = ECHO_PIN_LEFT;
  long duration;
  float distance;

  if (side == "RIGHT") {
    trigPin = TRIG_PIN_RIGHT;
    echoPin = ECHO_PIN_RIGHT;
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

  if (side == "RIGHT") {
    distance_right = distance;
  } else {
    distance_left = distance;
  }
  
//
//  if (distance == 0 or distance > MAX_DISTANCE) distance = MAX_DISTANCE;

}
