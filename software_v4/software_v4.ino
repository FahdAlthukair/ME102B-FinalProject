#include <Arduino.h>
#include <HardwareSerial.h>

//Thermal camera attributes
HardwareSerial MySerial(1);
uint8_t arrayCenter = 10;

//Initialize IR 
#define IR_PIN_LEFT 26
#define IR_PIN_RIGHT 25


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

int stop_distance = 1000;
const int STOP_DISTANCE_1 = 1000;
const int STOP_DISTANCE_2 = 900;


// current IR distance reading
float distance_right = 0;
float distance_left = 0;

// prev IR distance reading
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

  pinMode(IR_PIN_LEFT, INPUT);
  pinMode(IR_PIN_RIGHT, INPUT);

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

  
  delay(100);
  

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
    readIR();
    if (arrayCenter > 2 && arrayCenter < 7 || (arrayCenter == 10) && (distance_left > 400 || distance_right > 400)) {
      if (distance_left < stop_distance && distance_right < stop_distance) {
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
    if (arrayCenter < 3) {
       readIR();
      if (distance_left < stop_distance && distance_right < stop_distance) {
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
    if (arrayCenter != 10 && arrayCenter > 6) {
      readIR();
      if (distance_left < stop_distance && distance_right < stop_distance) {
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

void readIR() {
  distance_right = 0;
  distance_left = 0;
  for (int i = 0; i < 25; i++) {
    distance_right += analogRead(IR_PIN_RIGHT);
    distance_left += analogRead(IR_PIN_LEFT);
  }
  distance_right /= 25;
  distance_left /= 25;
}
