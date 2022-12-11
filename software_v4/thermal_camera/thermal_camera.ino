#include <Adafruit_MLX90640.h>

Adafruit_MLX90640 mlx;
float frame[32*24]; // buffer for full frame of temperatures

// uncomment *one* of the below
//#define PRINT_TEMPERATURES
#define PRINT_ASCIIART

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial1.begin(9600);

  Serial.println("Adafruit MLX90640 Simple Test");
  if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("MLX90640 not found!");
    while (1) delay(10);
  }
  Serial.println("Found Adafruit MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);
  
  //mlx.setMode(MLX90640_INTERLEAVED);
  mlx.setMode(MLX90640_CHESS);
  Serial.print("Current mode: ");
  if (mlx.getMode() == MLX90640_CHESS) {
    Serial.println("Chess");
  } else {
    Serial.println("Interleave");    
  }

  mlx.setResolution(MLX90640_ADC_18BIT);
  Serial.print("Current resolution: ");
  mlx90640_resolution_t res = mlx.getResolution();
  switch (res) {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }

  mlx.setRefreshRate(MLX90640_2_HZ);
  Serial.print("Current frame rate: ");
  mlx90640_refreshrate_t rate = mlx.getRefreshRate();
  switch (rate) {
    case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
    case MLX90640_1_HZ: Serial.println("1 Hz"); break; 
    case MLX90640_2_HZ: Serial.println("2 Hz"); break;
    case MLX90640_4_HZ: Serial.println("4 Hz"); break;
    case MLX90640_8_HZ: Serial.println("8 Hz"); break;
    case MLX90640_16_HZ: Serial.println("16 Hz"); break;
    case MLX90640_32_HZ: Serial.println("32 Hz"); break;
    case MLX90640_64_HZ: Serial.println("64 Hz"); break;
  }
  delay(3000);
  while (!Serial1) {
    Serial.println("Waiting for Serial1");
    delay(1);
  }
Serial.println("Found Serial1");
Serial1.begin(9600);
  
}

void loop() {
  delay(50);
  if (mlx.getFrame(frame) != 0) {
    Serial.println("Failed");
    return;
  }
  Serial.println();
  Serial.println();
   float posSum = 0;
   float maxSum = 0;
   float minSum = 1000;
   float sum = 0;
   int humanLoc = 0;
   for (uint8_t w=0; w<32; w++) {
     float t = 0;
     // For each column add the rows (heat signatures)
     for (uint8_t h=0; h<24; h++) {
       t += frame[h*32 + w];
     }
     //Get the average heat sinature for that column
      t /= 24;
      
      //For every 4 columns find the average heat signature for the four and check if its a min or max
      if ( w % 4 == 0 && w != 0) {
        posSum /= 4;
        if (maxSum < posSum) {
          maxSum = posSum;
          humanLoc = w / 4;
        }

        if (minSum > posSum) {
          minSum = posSum;
        }
        posSum = 0;
      }
      posSum += t;
      sum += t;
    }

// Find the average heat signature for the last 4 columns
  posSum /= 4;
  if (maxSum < posSum) {
    maxSum = posSum;
    humanLoc = 8;
  }

  if (minSum > posSum) {
    minSum = posSum;
  }

  //Find the difference between the max average and min average for each 4 column segment
  //Send the location of the max segment (1-8) if difference between the max and min is more than 3
  float diff = abs(maxSum - minSum);
  if (diff > 3) {
      Serial1.write(humanLoc);
      Serial.print(humanLoc);
      Serial.println("sending humanLoc");
  } else {
    Serial1.write(10);
    Serial.println("No Human or too close sending 0");
  }

}
