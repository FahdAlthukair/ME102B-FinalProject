#include <Adafruit_MLX90640.h>

Adafruit_MLX90640 mlx;
float frame[32*24]; // buffer for full frame of temperatures

// uncomment *one* of the below
//#define PRINT_TEMPERATURES
#define PRINT_ASCIIART

void setup() {
  while (!Serial) delay(10);
  Serial.begin(115200);
  delay(100);

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
   float avg = 0;
   int humanLoc = 0;
   float first_col = 0;
   for (uint8_t w=0; w<32; w++) {
     float t = 0;
     for (uint8_t h=0; h<24; h++) {
       t += frame[h*32 + w];
     }
      t /= 24;
      if (w == 0) {
        first_col = t;
      }
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
        
//    Serial.print(t/24);
//    Serial.print(", ");
    sum += t;
    }

  posSum /= 4;
  if (maxSum < posSum) {
    maxSum = posSum;
    humanLoc = 8;
  }

  if (minSum > posSum) {
    minSum = posSum;
  }
//  Serial.print("max sum is: ");
//  Serial.println(maxSum);
//  //avg = sum/32;
//  Serial.print("minSum is: ");
//  Serial.println(minSum);
  float diff = abs(maxSum - minSum);
//  Serial.print("first col is: ");
//  Serial.println(first_col);
//  Serial.print("diff is: ");
//  Serial.println(diff);
  if (diff > 3) {
      Serial.print(humanLoc);
      Serial.println();
    //Serial.println("No Human or too close");
  } else {
    Serial.println("No Human or too close");
  }

}
