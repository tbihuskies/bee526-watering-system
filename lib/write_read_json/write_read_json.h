// sensor_read.h

#include <string>
#ifndef WRITE_READ_JSON_H
#define WRITE_READ_JSON_H
#include <iostream>


namespace WaterSoil
{

    enum class Month {JANUARY = 1, FEBRURARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER};

    void readJSON(String filename);
    void createFirstJSONFile(String filename);
    // void writeJSON(std::string filename);
    
}


#endif