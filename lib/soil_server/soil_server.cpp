// soil_server.cpp
#include <Arduino.h>
#include "soil_server.h"
#include <WebServer.h>
#include <Preferences.h>
#include <ESP32WifiNtp.h>
#include "func_pointers.h" // for function pointer onIntervalChange

WebServer server(80);
Preferences _prefs; // used to update the prefs

void serverSetup(Preferences &pf) {
    server.begin(); // start the server
    Serial.println("HTTP server started on port 80");
    // add server-on's to handle HTTP requests
    server.on("/updatePlantInfo", handleUpdatePlant);
    server.on("/updateRefTime", handleUpdateRefTime);
    server.on("/manualWater", handleManuallyWater);
    server.on("/testString", printTestString);
    server.on("/setChkInterval", handleSetCheckInterval); // this will be used to set the check interval for watering plants

    // handles undefined requests
    server.onNotFound([]() {
        Serial.println("Request not found.");
        server.send(404, "text/plain", "Request not Found");
    });
    
    _prefs = pf;
    Serial.println("Server setup complete.");
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
    if (onManuallyUpdateRefTime) {
        onManuallyUpdateRefTime(); // call function pointer in func_pointers.h
    }
    // _prefs.putString("date", getTimeStampString(timeClient)); // updates the date to the current time
    server.send(200, "text/plain", "Reference time updated successfully.");
} 

// handles when the user chooses to manually water 
void handleManuallyWater(){
    if (server.hasArg("plantNum")) {
        int plantNum = server.arg("plantNum").toInt();
        if (plantNum < 1 || plantNum > 3) {
            Serial.println("Invalid plant number provided.");
            server.send(400, "text/plain", "Invalid plant number sent.");
            return;
        }
        Serial.printf("Manually watering plant #%d...\n", plantNum);
        const int relayOutputPins[] = {25, 26, 27};
        pinMode(relayOutputPins[plantNum - 1], OUTPUT); // sets corresponding pin to output for relay
        digitalWrite(relayOutputPins[plantNum - 1], HIGH); // turn on the relay for 5 seconds
        delay(5000);
        digitalWrite(relayOutputPins[plantNum - 1], LOW); // turn off relay

    } else {
        Serial.println("No plant number provided for manual watering.");
        server.send(400, "text/plain", "No plant number provided.");
        return;
    }
    Serial.println("Manual watering complete.");
        if (onManuallyUpdateRefTime) { // after manually watering, update reference time in both main.cpp and in Preferences
            onManuallyUpdateRefTime(); // call function pointer in func_pointers.h
        }
    server.send(200, "text/plain", "Plant manual watering complete.");
}

void handleSetCheckInterval() {
    if (server.hasArg("interval")) {
        String interval = server.arg("interval");
        if (interval == "15mins") {
            _prefs.putInt("check-interval", 900); // stores interval duration in seconds
            Serial.println("Check interval set to 15 minutes.");
            server.send(200, "text/plain", "Check time interval set to 15 minutes.");
        } else if (interval == "30mins") {
            _prefs.putInt("check-interval", 1800);
            Serial.println("Check interval set to 30 minutes.");
            server.send(200, "text/plain", "Check time interval set to 30 minutes.");
        } else if (interval == "1hr") {
            _prefs.putInt("check-interval", 3600); 
            Serial.println("Check interval set to 1 hour.");
            server.send(200, "text/plain", "Check time interval set to 1 hour.");
        } else if (interval == "2hrs") {
            _prefs.putInt("check-interval", 7200);
            Serial.println("Check interval set to 2 hours.");
            server.send(200, "text/plain", "Check time interval set to 2 hours.");
        } else if (interval == "6hrs") {
            _prefs.putInt("check-interval", 21600);
            Serial.println("Check interval set to 6 hours.");
            server.send(200, "text/plain", "Check time interval set to 6 hours.");
        } else if (interval == "12hrs") {
            _prefs.putInt("check-interval", 43200);
            Serial.println("Check interval set to 12 hours.");
            server.send(200, "text/plain", "Check time interval set to 12 hours.");
        } else if (interval == "24hrs") {
            _prefs.putInt("check-interval", 86400);
            Serial.println("Check interval set to 24 hours.");
            server.send(200, "text/plain", "Check time interval set to 24 hours.");
        }  else {
            Serial.println("Invalid check interval provided.");
            server.send(400, "text/plain", "Invalid check interval. Not updated");
        }
        int intervalSet = _prefs.getInt("check-interval", 3600); // get the updated check interval, returns 1 hour if not set
        if (onIntervalChange) {
            onIntervalChange(intervalSet); // call the function pointer in func_pointers.h
        }
    }
}