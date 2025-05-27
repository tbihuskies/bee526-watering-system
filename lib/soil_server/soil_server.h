#ifndef SOIL_SERVER_H
#define SOIL_SERVER_H

#include <Preferences.h>
#include <NTPClient.h>

void serverSetup(Preferences &pf);
void handleHttpRequests();

void printTestString();
void handleUpdatePlant();
void handleUpdateRefTime();
void handleManuallyWater();
void handleSetCheckInterval();

#endif