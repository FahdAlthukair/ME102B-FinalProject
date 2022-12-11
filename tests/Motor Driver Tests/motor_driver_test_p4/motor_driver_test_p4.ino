//Initialize Motors
#define ENA 13
#define IN1 12
#define IN2 27
#define IN3 33
#define IN4 15
#define ENB 32


// setting PWM properties
const int MAX_PWM_VOLTAGE = 255;
const int NOM_PWM_VOLTAGE = 150;
const int MIN_PWM_VOLTAGE = 20;

// Motor timing
unsigned long motorCm;
unsigned long motorPm;
const unsigned long MOTOR_PERIOD = 20;

//button setup
#define BTN 26  // declare the button pin number

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

bool state = false;

void setup() {
  Serial.begin(115200); // Starts the serial communication
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
}

void loop() {

 if (checkButton()) {
  state = !state;
  Serial.print("state is: ");
  Serial.println(state);
  
 }
 Serial.println("Here");
 if (state) {
   analogWrite(ENA,255);
   analogWrite(ENB, 255);
   digitalWrite(IN1,HIGH);
   digitalWrite(IN2,LOW);
   digitalWrite(IN3,HIGH);
   digitalWrite(IN4,LOW);
   delay(2000);
   digitalWrite(IN2,HIGH);
   digitalWrite(IN1,LOW);
   digitalWrite(IN4,HIGH);
   digitalWrite(IN3,LOW);
   delay(2000);
 } else {
   digitalWrite(IN2,LOW);
   digitalWrite(IN1,LOW);
   digitalWrite(IN4,LOW);
   digitalWrite(IN3,LOW);
 }
}

bool checkButton() {
  if (buttonIsPressed && timePassed) {
    return true;
  }
  return false;
}


// motor control function declarations

void forward()
{
 for(int i=0;i<=100;i++)
 {
 analogWrite(ENA,i);
 analogWrite(ENB,i);
 digitalWrite(IN1,HIGH);
 digitalWrite(IN2,LOW);
 digitalWrite(IN3,LOW);
 digitalWrite(IN4,HIGH);
 }
}

void reverse()
{
 for(int j=0;j<=100;j++)
 {
 analogWrite(ENA,j);
 analogWrite(ENB,j);
 digitalWrite(IN1,LOW);
 digitalWrite(IN2,HIGH);
 digitalWrite(IN3,HIGH);
 digitalWrite(IN4,LOW);
 }
}

void left()
{
 analogWrite(ENA,127);
 analogWrite(ENB,0);
 digitalWrite(IN1,HIGH);
 digitalWrite(IN2,LOW);
 digitalWrite(IN3,HIGH);
 digitalWrite(IN4,HIGH);
}

void right()
{
 analogWrite(ENA,0);
 analogWrite(ENB,127);
 digitalWrite(IN1,HIGH);
 digitalWrite(IN2,HIGH);
 digitalWrite(IN3,LOW);
 digitalWrite(IN4,HIGH);
}

void stoprobot()
{
 analogWrite(ENA,0);
 analogWrite(ENB,0);
 digitalWrite(IN1,LOW);
 digitalWrite(IN2,LOW);
 digitalWrite(IN3,LOW);
 digitalWrite(IN4,LOW);
}
