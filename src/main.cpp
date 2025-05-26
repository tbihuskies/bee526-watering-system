#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "LittleFS.h"
#include <Preferences.h>
#include <WiFi.h>
#include <WebServer.h>
// #include <iostream>

#include "ESP32WifiShowNetworks.h"
#include "ESP32WifiNtp.h"
#include "littlefs_funcs.h"
#include "soil_server.h"

using namespace LittleFSFuncs;

#define FORMAT_LITTLEFS_IF_FAILED true


// SD Card pin definitions
#define SD_MISO 19
#define SD_MOSI 23
#define SD_CLK 18
#define SD_CS 5



// put function declarations here:
void setCheckInterval(void);


// SSID and password of Wifi connection:
const char* ssid = "Shuttle";
const char* password = "!0243545292";
const int moistInputPins[] = {35, 32, 33}; // moisture analog pins
const int relayOutputPins[] = {};
// const int pumpOutPin;

// put enums & constants here:
const int NUM_PLANTS = 3;

enum CheckTimeInterval {
  T_15MINS = 900, T_30MINS = 1800, T_1HR = 3600, T_2HRS = 7200, T_6HRS = 21600, T_12HRS = 43200, T_24HRS = 86400
};
enum MoistureCategory {DRY=1, MEDIUM_DRY=2, MEDIUM=3, WET=4};

// put non-constants here
Preferences prefs;


// BluetoothSerial SerialBT;

int sensorReading1;
int sensorReading2;
int sensorReading3;
CheckTimeInterval timeInterval;


void setup() {

  Serial.begin(115200); // initialize

  // set up file system
  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {Serial.println("LittleFS mount failed.");} else {Serial.println("LittleFS mount succeeded");}


  // sd card setup
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return;
  }

  // sensor setup
  // analogSetAttenuation(ADC_11db);

  // set up Wifi connection
    
    scanNetworks();                       // scan for all WiFi networks
    connectToNetwork(ssid, password);     // try to connect to SSID defined by user
    
    Serial.println(WiFi.macAddress());    // print MAC address of WiFi interface
    Serial.println(WiFi.localIP());       // print IP address of WiFi interface

    // initialize NTP
    ntpSetup();
    Serial.println(getTimeStampString());


  // create JSON file w/ thresholds if it doesn't exist yet
  //  {
  //   Serial.println("Open attempt failed");
  //   WaterSoil::createFirstJSONFile(filename);
  // }

  // set up preferences
  prefs.begin("watered_last", false); // sets up time last watered.
  prefs.putString("date", "MM-DD-YYYY HH:MM:SS"); // default value
  prefs.end();
  
  // set preferences w/ default values for plants
  prefs.begin("plant1", false);
  prefs.putString("name", "Plant 1");
  prefs.putString("type", "Type 1");
  prefs.putInt("moisture-cat", 1); // 1 corresponds to dry; 4 to wet
  prefs.end();

  prefs.begin("plant2", false);
  prefs.putString("name", "Plant 2");
  prefs.putString("type", "Type 2");
  prefs.putInt("moisture-cat", 2);  
  prefs.end();

  prefs.begin("plant3", false);
  prefs.putString("name", "Plant 3");
  prefs.putString("type", "Type 3");
  prefs.putInt("moisture-cat", 3);
  prefs.end();

  // initiate the mini-server passing Preferences
  serverSetup(prefs);


}

void loop() {

  // put your main code here, to run repeatedly:

  // if elapsed time has already passed (i.e. time >= interval + time last watered) 
  if (true) {// replace 
    int moistReadings[] = {0, 0, 0}; // initialize for later storage
    for (int i=0; i<NUM_PLANTS-1; i++) {
    // read output & convert to moisture percentage based on calibration
      int sensorReading = analogRead(moistInputPins[i]);
      moistReadings[i] = sensorReading;

    }
  }
  
  // use this and the soil_server lib to handle incoming requests from app
  handleHttpRequests();

}


// put function definitions here:
void setCheckInterval(void){

}