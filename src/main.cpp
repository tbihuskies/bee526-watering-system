#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "LittleFS.h"
#include <Preferences.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WebServer.h>
// #include <iostream>

#include "ESP32WifiShowNetworks.h"
#include "ESP32WifiNtp.h"
#include "soil_server.h"
#include "func_pointers.h"

#define FORMAT_LITTLEFS_IF_FAILED true


// SD Card pin definitions if custom parameters needed
// #define SD_MISO 19
// #define SD_MOSI 23
// #define SD_CLK 18
// #define SD_CS 5

// put function declarations here:

// SSID and password of Wifi connection:
const char* ssid = "xxxxx";
const char* password = "password";
const int moistInputPins[] = {39, 34, 35}; // moisture analog pins
const int relayOutputPins[] = {25, 26, 27};
// const int pumpOutPin;

// put enums & constants here:
const int NUM_PLANTS = 3;

enum CheckTimeInterval { // time intervals with ints in seconds
  T_15MINS = 900, T_30MINS = 1800, T_1HR = 3600, T_2HRS = 7200, T_6HRS = 21600, T_12HRS = 43200, T_24HRS = 86400
};
enum MoistureCategory {DRY=1, MEDIUM_DRY=2, MEDIUM=3, WET=4};

// put non-constants here
Preferences prefs;
NTPClient timeClient = ntpSetup(); // NTP client to get time
long refTimeInMillis = 0; // comparison point time, used with millis() to determine when to check moisture 
CheckTimeInterval timeInterval = T_1HR; // default time interval for checking moisture

// function pointers here

// called in soil_server.cpp when interval is updated from app
void updateInterval(int intervalSet) {
  timeInterval = static_cast<CheckTimeInterval>(intervalSet);
  Serial.println("Interval updated.");
}
void (*onIntervalChange)(int intervalSet) = updateInterval;



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
    Serial.println("NTP Client has been set up. Current time is:");
    Serial.println(getTimeStampString(timeClient));


  // create JSON file w/ thresholds if it doesn't exist yet
  //  {
  //   Serial.println("Open attempt failed");
  //   WaterSoil::createFirstJSONFile(filename);
  // }

  // set up preferences
  prefs.begin("water-times", false); // sets up time last watered.
  prefs.putString("date", getTimeStampString(timeClient)); // default value
  prefs.putInt("check-interval", timeInterval); // default time interval
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
  serverSetup(prefs, timeClient);


}

void loop() {

  // put your main code here, to run repeatedly:

  // if elapsed time has already passed (i.e. time >= interval + time last watered)

  if (millis() - refTimeInMillis >= timeInterval * 1000) { // if time interval has passed since last check
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

