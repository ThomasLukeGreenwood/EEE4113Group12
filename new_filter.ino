/********************************************
Rumbidzai Mashumba
Load cell filter
Created On: 05/05/2024

********************************************/
#include <SD.h>  // Include the SD library
#include <HX711.h> // Include the HX711 library
#include <esp_sleep.h> // Include the ESP32 sleep library



#define LOADCELL_DOUT_PIN 16//choose the right pins
#define LOADCELL_SCK_PIN 4

#define Threshold 1   // if loadcell measures anything more wake up mcu

#define WAKEUP_PIN 33 // GPIO pin to trigger wakeup from deep sleep



HX711 scale; // HX711 load cell instance

//calibration factor
float calibrationFactor = -37.2267; //calculate calibration factor


// SD card pins
const int SD_CS_PIN = 21;   //check the SD pin


float x[] = {0,0,0};
float y[] = {0,0,0};
int k = 0;

float prevWeight[3] = {0.0};
float prevOutput[3] = {0.0};

float currentWeight = 0.0;

bool isRecording = false;
//float a[] = {1,-3.91791462, 5.75709608, -3.76036868, 0.92118815};
//float b[] = {3,73e-9, 1.491e-8, 2.237e-8, 3.73e-9};

float t = micros()/1.0e6;

  // Compute the filtered signal
  // (second order Butterworth example)
float b[] = {0.00024132, 0.00048264, 0.00024132};
float a[] = {1.95558189, -0.95654717};

const int DC_SIGNAL = 5; // Define the DC signal value
const int NOISE_AMPLITUDE = 2; // Define the amplitude of the noise
int random_noise;
int signal_sample;


// Sleep duration in microseconds (1 second = 1000000 microseconds)
const uint64_t SLEEP_DURATION_US = 10 * 1000000; // 10 seconds

void setup() {

  pinMode(WAKEUP_PIN, INPUT_PULLDOWN); // Configure the wakeup pin as an input with pull-up resistor
  Serial.println("Load Cell initializing...");
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_gain(128);
  delay(1000);

  initialisation();


//SD connection check
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Failed to initialize SD card!");
  } else {
    Serial.println("SD card initialized.");
  }


  Serial.println("HX711 initialized. Place an object on the load cell.");
   
  //sleep();
  wakeupInterrupt();
  //attachInterrupt(digitalPinToInterrupt(WAKEUP_PIN), wakeupInterrupt, RAISING);

}

void initialisation() {
  scale.tare();
  Serial.println("Scale Zeroed");
  delay(1000);
  scale.set_scale(calibrationFactor);
  Serial.println("Scale Calibration Factor set to:  ");
  Serial.println(calibrationFactor);
}


void wakeupInterrupt() {


  

    if (scale.is_ready()) {
      Serial.println("Put weight");
      delay(50);

      

      // signal_sample = scale.get_units(5);
      currentWeight = scale.get_units(5);

      // isRecording = True;
      // prevWeight[0] = 5;
      //while (true) {
        


          //currentWeight = scale.get_units(5);
  if (currentWeight < 1000) {
    Serial.println("Napping...");
    delay(1000); // Add a delay to ensure Serial output is transmitted
    esp_deep_sleep(SLEEP_DURATION_US); // Start deep sleep with the specified duration
   


  } else{
    Serial.print("Wakingup");
    while(1){
          // random_noise = rand() % (2 * NOISE_AMPLITUDE + 1) - NOISE_AMPLITUDE; // Generate random noise
          // int signal_sample = DC_SIGNAL + random_noise; // Add noise to the DC signal
          // signal_sample = 4 + 0.5*sin(2*PI*25*t) + sin(2*PI*75*t);
          signal_sample = scale.get_units(5);
          prevWeight[0] = signal_sample;
          prevOutput[0] = a[0] * prevOutput[1] + a[1] * prevOutput[2] +
                          b[0] * prevWeight[0] + b[1] * prevWeight[1] + b[2] * prevWeight[2];

          // prevWeight [0] = signal_sample;

          if (k % 3 == 0) {

            // For the serial monitor
            Serial.print(2 * prevWeight[0]);
            Serial.print(" ");
            Serial.println(2 * prevOutput[0]);
          }

          // Update time
          t += 0.001; // Increment time by 1ms

          delay(1); // Wait 1ms
          for (int i = 1; i >= 0; i--) {
            prevWeight[i + 1] = prevWeight[i]; // store xi
            prevOutput[i + 1] = prevOutput[i]; // store yi
          }

          k = k + 1;
        }
      }
   }

}

void loop() {
// if (scale.is_ready()) {
//     Serial.println("Put weight");
//     delay(5);
  
//   //signal_sample=scale.get_units(5);
//   //currentWeight=scale.get_units(5);

//   //if (currentWeight>0){
//     //isRecording = True;
//     //prevWeight[0] = 5;
//    while (true){

//     random_noise = rand() % (2 * NOISE_AMPLITUDE + 1) - NOISE_AMPLITUDE; // Generate random noise
//     //int signal_sample = DC_SIGNAL + random_noise; // Add noise to the DC signal
//     signal_sample= 4 + 0.5*sin(2*PI*25*t)+sin(2*PI*75*t);
//     //signal_sample=scale.get_units(5);
//     prevWeight[0]=signal_sample;
//     prevOutput[0] = a[0]*prevOutput[1] + a[1]*prevOutput[2] +
//                b[0]*prevWeight[0] + b[1]*prevWeight[1] +b[2]*prevWeight[2];
    
    

//     prevWeight [0]= signal_sample;
    
//     if(k % 3 ==0)
//   {
   
//     // For the serial monitor
//     Serial.print(2*prevWeight[0]);
//     Serial.print(" ");
//     Serial.println(2*prevOutput[0]);
//   }

//   // Update time
//   t += 0.001; // Increment time by 1ms

//   delay(1); // Wait 1ms
//     for(int i = 1; i >= 0; i--){
//       prevWeight[i+1] = prevWeight[i]; // store xi
//      prevOutput[i+1] =prevOutput[i]; // store yi
//     }

//   k = k+1;


// }
// //Serial.print("Napping");
// //esp_deep_sleep_start(SLEEP_DURATION_SECONDS * 1000000); // Go back to sleep

// }

}





