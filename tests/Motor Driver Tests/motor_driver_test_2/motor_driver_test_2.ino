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

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void loop() {
  
 analogWrite(ENA,180);
 analogWrite(ENB, 180);
 digitalWrite(IN1,HIGH);
 digitalWrite(IN2,LOW);
 digitalWrite(IN3,HIGH);
 digitalWrite(IN4,LOW);
 delay(100);
  digitalWrite(IN1,LOW);
 digitalWrite(IN2,LOW);
 digitalWrite(IN3,LOW);
 digitalWrite(IN4,LOW);
  analogWrite(ENA,0);
 analogWrite(ENB, 0);
 delay(1000);
// analogWrite(ENA,155);
// analogWrite(ENB, 155);
// delay(2000);
// digitalWrite(IN2,HIGH);
// digitalWrite(IN1,LOW);
// digitalWrite(IN4,HIGH);
// digitalWrite(IN3,LOW);
// delay(2000);
// analogWrite(ENA,255);
// analogWrite(ENB, 255);
// delay(2000);
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
