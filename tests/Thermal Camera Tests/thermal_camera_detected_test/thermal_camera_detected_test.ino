//Thermal Camera ------------------------------------------------------------------------------------
#include <Adafruit_MLX90640.h>
Adafruit_MLX90640 mlx;

bool printbool = true;
const byte MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(3000);

  //Thermal Camera Setup ------------------------------------------------------------------------------------
  Wire.setClock(400000); //Increase I2C clock speed to 400kHz - goes easy up to 1MHz
  Serial.println("Adafruit MLX90640 Simple Test");
  if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("MLX90640 not found!");
    while (1) delay(10);
  }
  Serial.println("Found Adafruit MLX90640");

}

void loop() {
  // put your main code here, to run repeatedly:
  if (printbool) {
    Serial.println("Hello");
    printbool = false;
  }
}








//Returns true if the MLX90640 is detected on the I2C bus
boolean isConnected(){
  Wire.beginTransmission((uint8_t)MLX90640_address);
  if (Wire.endTransmission() != 0)
    return (false); //Sensor did not ACK
  return (true);
}
