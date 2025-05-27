// Source code adapted from: (was originally written for ESP8266): 
// https://lastminuteengineers.com/esp8266-ntp-server-date-time-tutorial/

// IMPORTANT: install NTPclient library from Fabrice Weinberg in order to run this code (available in the Arduino IDE under Sketch -> Include Library -> Manage Libraries
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32WifiNtp.h>

#define UTC_OFFSET_IN_SECONDS -28800         // offset from GMT; here defined for US Pacific Std Time

// SSID and password of Wifi connection:

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_IN_SECONDS);

void ntpSetup(){
  // Wait for WiFi connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay (500);
    Serial.print ( "." );
  }
  timeClient.begin();
  timeClient.update();
}

long getEpochTime() {
  // returns the epoch time in seconds
  return timeClient.getEpochTime();
}

String getTimeStampString() {
   time_t rawtime = timeClient.getEpochTime();
   struct tm * ti;
   ti = localtime (&rawtime);

   uint16_t year = ti->tm_year + 1900;
   String yearStr = String(year);

   uint8_t month = ti->tm_mon + 1;
   String monthStr = month < 10 ? "0" + String(month) : String(month);

   uint8_t day = ti->tm_mday;
   String dayStr = day < 10 ? "0" + String(day) : String(day);

   uint8_t hours = ti->tm_hour;
   String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

   uint8_t minutes = ti->tm_min;
   String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

   uint8_t seconds = ti->tm_sec;
   String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

   return monthStr + "-" + dayStr + "-" + yearStr + " " +
          hoursStr + ":" + minuteStr + ":" + secondStr;
          // set to "MM-DD-YYYY HH:MM:SS" format
}

