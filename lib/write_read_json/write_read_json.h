// sensor_read.h

#include <string>
#ifndef WRITE_READ_JSON_H
#define WRITE_READ_JSON_H
#include <iostream>

using namespace std;


namespace WaterSoil
{

    void readJSON(std::string filename);
    void createFirstJSONFile(std::string filename);
    // void writeJSON(std::string filename);
    
}


#endif