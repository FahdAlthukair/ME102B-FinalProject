
//Thermal Camera ------------------------------------------------------------------------------------
#include <Adafruit_MLX90640.h>
Adafruit_MLX90640 mlx;
float frame[32*24]; // buffer for full frame of temperatures
// uncomment *one* of the below
#define PRINT_TEMPERATURES
//#define PRINT_ASCIIART
int T_human = 26;
int maxIndex = 0;
int maxCount = 0;
int maxHeight = 0;
int myindex = 0;
int myheight = 0;
int mycount = 0;
int totalhotcount = 0;
bool currentlyInRange = false;
bool humandetected = false;
int totalelements = 32;
int w_last = totalelements-1;
int arraycenter = 0;
unsigned long ThermalCameraTime;
unsigned int ThermalCameraDelay = 250;


void ThermalCameraSetup() {
  //Thermal Camera Setup ------------------------------------------------------------------------------------
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

  mlx.setRefreshRate(MLX90640_4_HZ);
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


//------------------------------------------------------------------------------------
void setup() {
Serial.begin(9600);
Serial1.begin(9600);

 
//Thermal Camera Setup ------------------------------------------------------------------------------------
 Wire.setClock(400000); //Increase I2C clock speed to 400kHz
ThermalCameraSetup();


}

//------------------------------------------------------------------------------------
void loop() {



   if (mlx.getFrame(frame) != 0) {
    Serial.println("Failed");
    return;
  }
  Serial.println();
  maxIndex = 0;
  maxHeight = 0;
  maxCount = 0;
  totalhotcount = 0; 

  
  for (uint8_t h=0; h<24; h++) {
                for (uint8_t w=0; w<32; w++) {
                float t = frame[(h*32) + w];
                 #ifdef PRINT_TEMPERATURES
                Serial.print(t, 1);
                Serial.print(", ");          
                #endif
                   if (t > T_human){
                     if (currentlyInRange == true){
                     mycount++;
                     totalhotcount++;
                                         
                      if (w == w_last){
                          currentlyInRange = false;
                          if (mycount > maxCount){
                              maxIndex = myindex;
                              maxHeight = myheight;
                              maxCount = mycount;  
                              }
                               } 
                                }
                          else {
                            currentlyInRange = true;
                            myindex = w;
                            myheight = h;
                            mycount = 1;     
                          }
                      }    
                      else {
                        currentlyInRange = false;
                        if (mycount > maxCount){
                         maxIndex = myindex;
                         maxHeight = myheight;
                         maxCount = mycount;  
                        }
                      }  
              }
     Serial.println();         
  }
    Serial.println();       
    arraycenter = maxIndex + (maxCount/2);
    Serial.print("  ");
    Serial.print("Total Hot Count: ");
    Serial.print(totalhotcount);
    Serial.print("  ");
    Serial.print("Max Count: ");
    Serial.print(maxCount);
    Serial.print("  ");
    Serial.print("Max Height: ");
    Serial.print(maxHeight);
    Serial.print("  ");
    Serial.print("Max Index: ");
    Serial.print(maxIndex);
    Serial.print("  ");
    Serial.print("Array Center: ");
    Serial.print(arraycenter);
    Serial.println();

    if (totalhotcount <= 6){
    arraycenter = 40;
    Serial1.write(arraycenter); 
    
    Serial.print("  ");
    Serial.print("Message Sent: No Human ");
    Serial.print("  ");
    Serial.println();
    }


    else if (totalhotcount > 6){   
    Serial1.write(arraycenter);
    Serial.print("  ");
    Serial.print("Message Sent: Human Sighted!");
    Serial.print("  ");
    Serial.println();
        }
}
 
