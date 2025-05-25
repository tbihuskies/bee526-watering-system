#include <Arduino.h>
#include <BluetoothSerial.h>
#include "FS.h"
#include "LittleFS.h"
#include "ArduinoJson.h"
#include <Preferences.h>
// #include <iostream>

#include "sensor_read.h"
#include "write_read_json.h"
#include "littlefs_funcs.h"

using namespace LittleFSFuncs;
using namespace WaterSoil;

#define FORMAT_LITTLEFS_IF_FAILED true

// // SD Card pin definitions
// #define SD_MISO 19
// #define SD_MOSI 23
// #define SD_CLK 18
// #define SD_CS 5


// put function declarations here:
void setCheckInterval(void);

// put enums & constants here:
const int NUM_PLANTS = 3;

enum CheckTimeInterval {
  T_15MINS = 900, T_30MINS = 1800, T_1HR = 3600, T_2HRS = 7200, T_6HRS = 21600, T_12HRS = 43200, T_24HRS = 86400
};

// put non-constants here
Preferences prefs;


BluetoothSerial SerialBT;

int sensorReading1;
int sensorReading2;
int sensorReading3;
CheckTimeInterval timeInterval;

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
  String filename = "/folder1/plantdata.txt";
  if (LittleFS.exists(filename)) {
    deleteFile(LittleFS, filename.c_str());
  }
  writeFile(LittleFS, filename.c_str(), "Viv");
  appendFile(LittleFS, filename.c_str(), "\nBar");
  readFile(LittleFS, filename.c_str());
  createDir(LittleFS, "/mydir");
  //  {
  //   Serial.println("Open attempt failed");
  //   WaterSoil::createFirstJSONFile(filename);
  // }

  // set up preferences
  // prefs.begin("watered_last", false); // sets up time last watered.
  // prefs.putString("date", )
  // prefs.end();
  
  // default values for plants
  prefs.begin("plant1", false);
  prefs.putString("name", "Plant 1");
  prefs.putString("type", "Type 1");
  prefs.putInt("moisture-hi", 50); // highest moisture percentage; will correspond to lower output
  prefs.putInt("moisture-lo", 20); // lowest moisture percentages; will correspond to higher output
  prefs.end();

  prefs.begin("plant2", false);
  prefs.putString("name", "Plant 2");
  prefs.putString("type", "Type 2");
  prefs.putInt("moisture-hi", 50); // highest moisture percentage; will correspond to lower output
  prefs.putInt("moisture-lo", 20); // lowest moisture percentages; will correspond to higher output
  prefs.end();

  prefs.begin("plant3", false);
  prefs.putString("name", "Plant 3");
  prefs.putString("type", "Type 3");
  prefs.putInt("moisture-hi", 50); // highest moisture percentage; will correspond to lower output
  prefs.putInt("moisture-lo", 20); // lowest moisture percentages; will correspond to higher output
  prefs.end();

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
  
  // When updating plant info from app
  // If incoming data comes in via Bluetooth
  if (SerialBT.available() && SerialBT.read() == 'w' ) {
    int updatedPlant = SerialBT.read(); // which plant (1, 2, or 3) has been updated from the app
    

  }


}


// put function definitions here:
void setCheckInterval(void){

}