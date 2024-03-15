#ifndef ESP_INT_INTERVALOMETER_H
#define ESP_INT_INTERVALOMETER_H

#include <ArduinoJson.h>

extern int actionIndex;
extern int totalActions;
extern bool isRunning;

extern unsigned long timeUntilNext();
extern void startSequence();
extern void stopSequence();
extern void loopSequence();

#endif
