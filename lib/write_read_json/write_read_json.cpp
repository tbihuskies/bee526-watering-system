// sensor_read.cpp

#include <Arduino.h>
#include <string>
// #include <ArduinoJson.h>
#include "write_read_json.h"
#include "sensor_read.h"
#include "littlefs_funcs.h"
#include <iostream>
#include "FS.h"
#include <LittleFS.h>
#include <errno.h>

#define FORMAT_LITTLEFS_IF_FAILED true

using namespace std;
using namespace LittleFSFuncs;

namespace WaterSoil {

    
    // void readJSON(string filename){


    // }

    // void createFirstJSONFile(string filename) {
    //      Serial.println("No data file exists; auto-generating:");
    //      createDir(LittleFS, "/folder1");
    //      writeFile(LittleFS, "/folder1/plantdata.txt", "");
         
    //      File file = LittleFS.open("/folder1/plantdata.txt");
    //       // create JSON Document
    //       JsonDocument jdoc;
    //       JsonObject jobj = jdoc.to<JsonObject>();
          
    //       JsonObject date_time_last_checked = jdoc["date_time_last_checked"].to<JsonObject>();
    //       date_time_last_checked["month"] = "May";
    //       date_time_last_checked["day"] = 20;
    //       date_time_last_checked["year"] = 2025;
    //       date_time_last_checked["hour"] = 15;
    //       date_time_last_checked["minute"] = 19;
          
    //       jdoc["interval"] = "1hr";

    //       JsonArray data = jdoc["data"].to<JsonArray>();

    //       JsonObject data_0 = data.add<JsonObject>();
    //       data_0["plantnumber"] = 1;
    //       data_0["name"] = "Hey";
    //       data_0["type"] = "Daffodil";

    //       JsonArray data_0_thresholds = data_0["thresholds"].to<JsonArray>();
    //       data_0_thresholds.add(30);
    //       data_0_thresholds.add(60);

    //       JsonObject data_1 = data.add<JsonObject>();
    //       data_1["plantnumber"] = 2;
    //       data_1["name"] = "Yoo";
    //       data_1["type"] = "Daisy";

    //       JsonArray data_1_thresholds = data_1["thresholds"].to<JsonArray>();
    //       data_1_thresholds.add(25);
    //       data_1_thresholds.add(50);

    //       JsonObject data_2 = data.add<JsonObject>();
    //       data_2["plantnumber"] = 3;
    //       data_2["name"] = "Gnarly";
    //       data_2["type"] = "Ficus";

    //       JsonArray data_2_thresholds = data_2["thresholds"].to<JsonArray>();
    //       data_2_thresholds.add(45);
    //       data_2_thresholds.add(70);

    //       jdoc.shrinkToFit();  // optional

    //       serializeJson(jdoc, file);

    }

    /* void writeJSON(string filename, int plant, string plantName, string plantType){
        

        


        // JsonDocument& jsd = new JsonDocument();
        // JsonObject& obj = jsd.createObject();

    } */

    // void writeDateTimeJSON(){}

}



