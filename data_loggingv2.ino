//Date: 09/05/2023
//Author: Francis Mutetwa
//Scale and Temperature Data logging script.

#include <SPI.h>
#include <SD.h>
#include "HX711.h"

// Pin definitions
const int CS_PIN = 5; // Chip Select pin for SD card module
const int TEMP_PIN = 2; // Analog pin for temperature sensor
const int LOADCELL_DOUT_PIN = 16; // HX711 DOUT pin
const int LOADCELL_SCK_PIN = 4; // HX711 SCK pin

HX711 scale; // Initialize HX711 instance

File tempFile; // File for temperature readings
File weightFile; // File for weight readings
File combinedFile; // File for combined data

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial connection

  Serial.println("Initializing SD card...");
  if (!SD.begin(CS_PIN)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialization done.");

  // Open files for writing
  tempFile = SD.open("/temperature.txt", FILE_WRITE);
  if (!tempFile) {
    Serial.println("Error opening temperature.txt");
    return;
  }

  weightFile = SD.open("/weight.txt", FILE_WRITE);
  if (!weightFile) {
    Serial.println("Error opening weight.txt");
    return;
  }

  combinedFile = SD.open("/combined_data.txt", FILE_WRITE); // Open combined data file
  if (!combinedFile) {
    Serial.println("Error opening combined_data.txt");
    return;
  }

  // Initialize the scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-24.546); // Calibration value for weight readings
  scale.tare(); // Reset the scale to zero
}

void loop() {
  // Read temperature
  float temperature = readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" degrees Celsius");

  // Record temperature
  recordTemperature(temperature);

  // Read weight
  float weight = readWeight();
  
  // Display weight in serial monitor
  if (weight < 24) {
    weight = 0;
  }
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" grams");

  // If weight is less than 24g, set it to 0 for recording
  if (weight < 24) {
    weight = 0;
  }

  // Record weight if it's greater than or equal to 350g
  if (weight >= 350) {
    recordWeight(weight);
    recordCombinedData(temperature, weight);
  }

  delay(1000); // Delay before next reading
}

float readTemperature() {
  int tempValue = analogRead(TEMP_PIN); // Read analog value from temperature sensor
  float voltage = tempValue / 1023.0; // Convert analog value to voltage (0-5V)
  return (voltage - 0.5) * 100; // Convert voltage to temperature in degrees Celsius
}

float readWeight() {
  return scale.get_units(1); // Get weight reading in grams from HX711
}

void recordTemperature(float temperature) {
  // Write temperature reading to file
  if (tempFile) {
    tempFile.print(getCurrentTimestamp()); // Write timestamp
    tempFile.print(", "); // Comma separated
    tempFile.print(temperature); // Write temperature to file
    tempFile.println(); // New line for next reading
    tempFile.flush(); // Flush the file buffer to ensure data is written
    Serial.println("Temperature recorded successfully.");
  } else {
    Serial.println("Error writing temperature to file.");
  }
}

void recordWeight(float weight) {
  // Write weight reading to file
  if (weightFile) {
    weightFile.print(getCurrentTimestamp()); // Write timestamp
    weightFile.print(", "); // Comma separated
    weightFile.print(weight); // Write weight to file
    weightFile.println(); // New line for next reading
    weightFile.flush(); // Flush the file buffer to ensure data is written
    Serial.println("Weight recorded successfully.");
  } else {
    Serial.println("Error writing weight to file.");
  }
}

void recordCombinedData(float temperature, float weight) {
  // Write combined data to file
  if (combinedFile) {
    combinedFile.print(getCurrentTimestamp()); // Write timestamp
    combinedFile.print(", "); // Comma separated
    combinedFile.print(temperature); // Write temperature value to file
    combinedFile.print(", "); // Comma separated
    combinedFile.println(weight); // Write weight value to file
    combinedFile.flush(); // Flush the file buffer to ensure data is written
    Serial.println("Combined data recorded successfully.");
  } else {
    Serial.println("Error writing combined data to file.");
  }
}

unsigned long getCurrentTimestamp() {
  return millis() / 1000; // Return current time in seconds since boot
}
