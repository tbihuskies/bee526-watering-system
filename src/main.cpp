#include <Arduino.h>
#include "FS.h"
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
const char* ssid = "Remember Ian's Domain";
const char* password = "seitanwrap";
const int moistInputPins[] = {39, 34, 35}; // moisture analog pins
const int relayOutputPins[] = {27, 26, 25}; // pins to turn on valve relays
const int pumpOutPin = 33; // pin for pump output

// put enums & constants here:
const int NUM_PLANTS = 3;
const int moistureThresholds[] = {3100, 2800, 2500, 2200}; // thresholds for dry, medium-dry, medium and wet categories respectively.
enum CheckTimeInterval { // time intervals with ints in seconds
  T_5MINS = 300, T_15MINS = 900, T_30MINS = 1800, T_1HR = 3600, T_2HRS = 7200, T_6HRS = 21600, T_12HRS = 43200, T_24HRS = 86400
};


// put non-constants here
Preferences prefs;
long refTimeInMillis = 0; // comparison point time, used with millis() to determine when to check moisture 
CheckTimeInterval timeInterval = T_1HR; // default time interval for checking moisture

// function pointers here

// called in soil_server.cpp when interval is updated from app
void updateInterval(int intervalSet) {
  timeInterval = static_cast<CheckTimeInterval>(intervalSet);
  Serial.println("Interval updated to " + String(timeInterval) + " seconds.");
}
void (*onIntervalChange)(int intervalSet) = updateInterval; // function pointer

void updateRefTime() {
  prefs.begin("water-times", false); // open "water times" namespace in Preferences
  prefs.putString("date", getTimeStampString()); // updates the date to the current time
  
  refTimeInMillis = millis(); // update reference time in this code, using current millis
  Serial.println("Reference time updated: " + prefs.getString("date"));
  prefs.end(); // close the namespace
}
void (*onManuallyUpdateRefTime)(void) = updateRefTime; // function pointer

long retrieveTimeRemainingInMillis() {
  return timeInterval * 1000 - (millis() - refTimeInMillis); // returns time remaining in millis
  // timeInterval * 1000 = time between checks in millis.
  // millis() - refTimeInMillis = time elapsed in millis since ref time set
}
long (*onRequestTimeRemaining)(void) = retrieveTimeRemainingInMillis; // function pointer


void setup() {

  Serial.begin(115200); // initialize


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
  if (!prefs.isKey("date")) { // if key does not exist, set it up
    Serial.println("Setting up 'water-times' default preferences.");
    prefs.putString("date", getTimeStampString()); // default value
    prefs.putInt("check-interval", timeInterval); // default time interval
  }  else {
    Serial.println("'water-times' preferences already set up.");
  }
  prefs.end();

  // set preferences w/ default values for plants
  prefs.begin("plant1", false); // set up plant 1
  if (!prefs.isKey("name")) { // if key does not exist, set it up
    Serial.println("Setting up 'plant-1' default preferences.");
    prefs.putString("name", "Plant 1");
    prefs.putString("type", "Type 1");
    prefs.putInt("moisture-cat", 1);  // 1 corresponds to dry; 4 to wet
  } else {
    Serial.println("'plant-1' preferences already set up.");
  }
  prefs.end();

  prefs.begin("plant2", false); // set up plant 2
  if (!prefs.isKey("name")) {
    Serial.println("Setting up 'plant-2' default preferences.");
    prefs.putString("name", "Plant 2");
    prefs.putString("type", "Type 2");
    prefs.putInt("moisture-cat", 2);  
  } else {
    Serial.println("'plant-2' preferences already set up.");
  }
  prefs.end();

  prefs.begin("plant3", false); // set up plant 3
  if (!prefs.isKey("name")) { 
    Serial.println("Setting up 'plant-3' default preferences.");
    prefs.putString("name", "Plant 3");
    prefs.putString("type", "Type 3");
    prefs.putInt("moisture-cat", 3);
  } else {
    Serial.println("'plant-3' preferences already set up.");
  }
  prefs.end();

  // initiate the mini-server passing Preferences
  serverSetup(prefs);
  refTimeInMillis = millis(); // set the reference time to current millis
  Serial.println("Reference time set.");
  Serial.println("Reference time set to: " + getTimeStampString());

}

void loop() {

  // put your main code here, to run repeatedly:

  // if elapsed time has already passed (i.e. time >= interval + time last watered)
  Serial.println("Current millis: " + String(millis() - refTimeInMillis));
  Serial.println("Time interval set to: " + String(timeInterval * 1000) + " milliseconds.");
  delay(2000);
  if ((millis() - refTimeInMillis) >= timeInterval * 1000) { // if time interval has passed since last check
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
        pinMode(pumpOutPin, OUTPUT); // set pump output pin to output
        digitalWrite(pumpOutPin, HIGH); // turn on the pump & wait 10 seconds
        delay(5000);
        pinMode(relayOutputPins[i], OUTPUT); // sets corresponding pin to output for relay
        digitalWrite(relayOutputPins[i], HIGH); // turn on the relay for 5 seconds
        delay(15000);
        digitalWrite(pumpOutPin, LOW); // turn off the pump
        digitalWrite(relayOutputPins[i], LOW); // turn off relay
        Serial.println("Watered plant #" + String(i+1));
      }
    }
    
  }
  
  // use this and the soil_server lib to handle incoming requests from app
  handleHttpRequests();

}

