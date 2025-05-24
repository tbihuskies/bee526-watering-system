#include <Arduino.h>
#include <BluetoothSerial.h>
#include "FS.h"
#include "sensor_read.h"
#include "write_read_json.h"
#include "LittleFS.h"
#include "ArduinoJson.h"
#include <iostream>


using namespace WaterSoil;

#define FORMAT_LITTLEFS_IF_FAILED true

// // SD Card pin definitions
// #define SD_MISO 19
// #define SD_MOSI 23
// #define SD_CLK 18
// #define SD_CS 5


// put function declarations here:
void readFile(fs::FS &fs, const char *path);


const int NUM_PLANTS = 3;

enum CheckTimeInterval {
  T_15MINS = 900, T_30MINS = 1800, T_1HR = 3600, T_2HRS = 7200, T_6HRS = 21600, T_12HRS = 43200, T_24HRS = 86400
};

CheckTimeInterval timeInterval;

int sensorReading;
BluetoothSerial SerialBT;
// SPIClass spi(VSPI);

int moistHighs[] = {500, 500, 500}; // array for storing high moisture value thresholds - recall a lower number represents more moist soil 
int moistLows[] = {2000, 2000, 2000}; // array for storing low moisture values - recall a higher number represents drier soil

void setup() {

  Serial.begin(115200); // initialize

  // set up file system
  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {Serial.println("LittleFS mount failed.");} else {Serial.println("LittleFS mount succeeded");}

  // set up SD card
  // SPI.begin(SD_CLK,SD_MISO,SD_MOSI,SD_CS);
  // if (!SD.begin(SD_CS)) {Serial.println("SD CARD mount fail. :( ");}
  // else {Serial.println("SD CARD mount success! :) ");}

  // set up bluetooth 
  char device_name[] = "We've got Bluetooth yo!";
  SerialBT.begin(device_name);
  char device_name_dblqts[75];
  sprintf(device_name_dblqts, "\"%s\"", device_name); // device name in double quotes
  Serial.print(device_name_dblqts);
  Serial.println(" has been started. It can now be paired via Bluetooth."); // message confirms Bluetooth connection

  // sensor setup
  // analogSetAttenuation(ADC_11db);
  // delay(2000);

  // create JSON file w/ thresholds if it doesn't exist yet
      std::string filename = "/folder1/plantdata.txt";
      Serial.println("Attempting to open...");
      readFile(LittleFS, "/folder1/plantdata.txt");
      //  {
      //   Serial.println("Open attempt failed");
      //   WaterSoil::createFirstJSONFile(filename);
      // }

  // get moisture thresholds from file
  for (int i=0; i<NUM_PLANTS-1; i++) {
      
  }
  

}

void loop() {


  // put your main code here, to run repeatedly:
  // sensorReading = analogRead(36);
  // Serial.print("Moisture reading is: ");
  // Serial.println(sensorReading);
  // delay(2000);

  // }
  
  // If incoming data comes in via Bluetooth
  if (SerialBT.available() && SerialBT.read() == 'w' ) {
    int updatedPlant = SerialBT.read(); // which plant (1, 2, or 3) has been updated from the app
    

  }
}


// put function definitions here:
void setCheckInterval(){

}

void readFile(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }
}