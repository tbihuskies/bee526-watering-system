// func_pointers.h for storing function pointers to use in other files

#ifndef FUNC_POINTERS_H
#define FUNC_POINTERS_H

extern void (*onIntervalChange)(int intervalSet); // function pointer for when the check interval changes
extern void (*onManuallyUpdateRefTime)(void); // function pointer for when the reference time is updated manually from app
extern long (*onRequestTimeRemaining)(void); // pointer for when time remaining til auto watering is requested

#endif