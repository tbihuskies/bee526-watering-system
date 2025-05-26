#ifndef WIFI_SHOW_NETWORKS_H
#define WIFI_SHOW_NETWORKS_H

#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>


void scanNetworks(void);

void connectToNetwork(const char* ssid, const char* password);



#endif