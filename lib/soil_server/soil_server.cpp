// soil_server.cpp
#include "soil_server.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

WebServer server(80);
Preferences _prefs; // used to update the prefs

void serverSetup(Preferences pf){
    server.begin(); // start the server
    Serial.println("HTTP server started on port 80");
    // add server-on's to handle HTTP requests
    server.on("/updatePlantInfo", handleUpdatePlant);
    server.on("/updateRefTime", handleUpdateRefTime);
    server.on("/manualWater", handleManuallyWater);
    server.on("/testString", printTestString);
    
    _prefs = pf;
}

void handleHttpRequests() {
    // handle client requests; is looped in main.cpp
    server.handleClient();
}

// test printing string to output
void printTestString() {
    if (server.hasArg("testString")) { // check if the server has a test string argument
        Serial.println("Test string received.");
        Serial.println(server.arg("testString")); // print the test string to the serial output
        server.send(200, "text/plain", "Test string successfully received & printed.");
    } else {
        Serial.println("No test string received.");
        server.send(400, "text/plain", "No test string provided.");
    }
}

// handles updating plant info from app
void handleUpdatePlant(){
    int plantNum = server.arg("plantNum").toInt(); // 1, 2, or 3
    String plant_namespace = "plant" + String(plantNum); // e.g. plant1, plant2, plant3 for preference setting purposes
    Serial.printf("Updating plant #%d\n", plantNum);
    _prefs.begin(plant_namespace.c_str(), false); // open namespace for this plant
    String requestString = ""; // stores info about which information was updated with request

    if (server.hasArg("name")) {
        // update name to server.arg("name")
        
        _prefs.putString("name", server.arg("name"));
        requestString += "Plant name updated successfully.\n";
    }
    if (server.hasArg("plantType")) {
        // update type to server.arg("plantType")
        _prefs.putString("type", server.arg("plantType"));
        requestString += "Plant type updated successfully.\n";
    }
    if (server.hasArg("moistCat")) { //update according to string given
        if (server.arg("moistCat").toInt() == 0) {
            Serial.println("Moisture category not convertible to int. Category not updated.");
            server.send(400, "text/plain", "Invalid moisture category");
            return;
        }
            switch (server.arg("moistCat").toInt()) {
                case 1: // DRY
                    _prefs.putInt("moisture-cat", 1);
                    requestString += "Moisture category updated successfully.\n";
                    break;
                case 2: // MEDIUM_DRY
                    _prefs.putInt("moisture-cat", 2);
                    requestString += "Moisture category updated successfully.\n";
                    break;
                case 3: // MEDIUM
                    _prefs.putInt("moisture-cat", 3);
                    requestString += "Moisture category updated successfully.\n";
                    break;
                case 4: // WET
                    _prefs.putInt("moisture-cat", 4);
                    requestString += "Moisture category updated successfully.\n";
                    break;
                default:
                    Serial.println("Invalid moisture category");
                    requestString += "Invalid moisture category; not updated.\n";
            }
        
    }
    _prefs.end(); // close the namespace
    if (requestString == "") {
        server.send(200, "text/plain", "No updates made to plant info.");
    } else {
        server.send(200, "text/plain", requestString);
    }
}

// handles when the time moisture last checked is manually updated
void handleUpdateRefTime(){
    // DateTime string will be in format MM-DD-YYYY HH:MM:SS
    // if (server.hasArg("timeManual")) {

    // }
    Serial.println("called handleUpdateRefTime");
    server.send(200, "text/plain", "Will program this handler later.");
} 

// handles when the user chooses to manually water 
void handleManuallyWater(){
    Serial.println("called handleManuallyWater");
    server.send(200, "text/plain", "Will program this handler later.");
} 