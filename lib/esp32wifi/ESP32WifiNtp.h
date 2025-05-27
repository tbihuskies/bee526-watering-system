#ifndef ESP32_WIFI_NTP_H
#define ESP32_WIFI_NTP_H

#include <NTPClient.h>

    NTPClient ntpSetup();
    long getEpochTime(NTPClient &timeClient);
    String getTimeStampString(NTPClient &timeClient);
 

#endif