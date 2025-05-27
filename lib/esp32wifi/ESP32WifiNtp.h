#ifndef ESP32_WIFI_NTP_H
#define ESP32_WIFI_NTP_H

#include <NTPClient.h>

extern NTPClient timeClient; // shared NTP client

    void ntpSetup();
    long getEpochTime();
    String getTimeStampString();

#endif