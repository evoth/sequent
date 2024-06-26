#ifndef SEQUENT_TIME_MILLIS_H
#define SEQUENT_TIME_MILLIS_H

#include <Arduino.h>
#include <TimeLib.h>

extern unsigned long msReset;
extern unsigned long msDelay;

extern unsigned long long fullTimeMs(bool useFull = true);

#endif