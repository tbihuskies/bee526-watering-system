#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "LittleFS.h"
#include <Preferences.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WebServer.h>

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
const int relayOutputPins[] = {25, 26, 27}; // pins to turn on valve relays
const int pumpOutPin = 33; // pin for pump output, if used

// put enums & constants here:
const int NUM_PLANTS = 3;
const int moistureThresholds[] = {3100, 2800, 2500, 2200}; // thresholds for dry, medium-dry, medium and wet categories respectively.
enum CheckTimeInterval { // time intervals with ints in seconds
  T_15MINS = 900, T_30MINS = 1800, T_1HR = 3600, T_2HRS = 7200, T_6HRS = 21600, T_12HRS = 43200, T_24HRS = 86400
};


// put non-constants here
Preferences prefs;
long refTimeInMillis = 0; // comparison point time, used with millis() to determine when to check moisture 
CheckTimeInterval timeInterval = T_1HR; // default time interval for checking moisture

// function pointers here

// called in soil_server.cpp when interval is updated from app
void updateInterval(int intervalSet) {
  timeInterval = static_cast<CheckTimeInterval>(intervalSet);
  Serial.println("Interval updated.");
}
void (*onIntervalChange)(int intervalSet) = updateInterval; // function pointer

void updateRefTime() {
  prefs.begin("water-times", false); // open "water times" namespace in Preferences
  prefs.putString("date", getTimeStampString()); // updates the date to the current time
  prefs.end(); // close the namespace

  refTimeInMillis = millis(); // update reference time in this code, using current millis
  Serial.println("Reference time updated.");
}
void (*onManuallyUpdateRefTime)(void) = updateRefTime; // function pointer


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
    
    ntpSetup(); // starts NTP client
    Serial.println("NTP Client has been set up. Current time is:");
    Serial.println(getTimeStampString());


  // create JSON file w/ thresholds if it doesn't exist yet
  //  {
  //   Serial.println("Open attempt failed");
  //   WaterSoil::createFirstJSONFile(filename);
  // }

  // set up preferences
  prefs.begin("water-times", false); // sets up time last watered.
  prefs.putString("date", getTimeStampString()); // default value
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
  serverSetup(prefs);


}

void loop() {

  // put your main code here, to run repeatedly:

  // if elapsed time has already passed (i.e. time >= interval + time last watered)

  if (millis() - refTimeInMillis >= timeInterval * 1000) { // if time interval has passed since last check
    Serial.println("Time last checked: " + getTimeStampString());
    refTimeInMillis = millis(); // update reference time to current millis
    prefs.begin("water-times", false);
    prefs.putString("date", getTimeStampString()); // update the date in Preferences w/ timestamp from NTP client
    prefs.end(); // close the namespace
    for (int i=0; i<NUM_PLANTS-1; i++) {      
      int sensorReading = analogRead(moistInputPins[i]);
      int plantThreshold; // designated threshold for plant; either 3100, 2800, 2500, or 2200 depending on moisture category
      String plantNum = "plant" + String(i+1); // plant1, plant2, plant3 for getting from Preferences
      prefs.begin(plantNum.c_str(), true); // open namespace for plant

      switch (prefs.getInt("moisture-cat", 0)) { // set threshold based on moisture category
        case 1:
          plantThreshold = moistureThresholds[0]; // dry: 3100
          break;
        case 2:
          plantThreshold = moistureThresholds[1]; // medium-dry: 2800
          break;
        case 3:
          plantThreshold = moistureThresholds[2]; // medium: 2500
          break;
        case 4:
          plantThreshold = moistureThresholds[3]; // wet: 2200
          break;
        default:
          plantThreshold = 3100; // invalid category
      }
      prefs.end();

      if (sensorReading > plantThreshold) {
        Serial.println("Watering plant #" + String(i+1) + ". Sensor reading is: " + String(sensorReading));
        pinMode(relayOutputPins[i], OUTPUT); // sets corresponding pin to output for relay
        digitalWrite(relayOutputPins[i], HIGH); // turn on the relay for 5 seconds
        delay(5000);
        digitalWrite(relayOutputPins[i], LOW); // turn off relay
        Serial.println("Watered plant #" + String(i+1));
      }
    }
    
  }
  
  // use this and the soil_server lib to handle incoming requests from app
  handleHttpRequests();

}

