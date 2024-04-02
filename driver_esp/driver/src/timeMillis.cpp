#include "timeMillis.h"

// Value of millis() when setTime() was last called
unsigned long msReset = 0;
// Time milliseconds when setTime() was last called
unsigned long msDelay = 0;

// Note: now() from TimeLib.h returns an integer number of seconds which is
// aligned to the value of millis() when setTime() was called. This compensates
// for that fact and adds an additional millisecond value.
unsigned long long fullTimeMs(bool useFull) {
  if (!useFull)
    return millis();
  return (unsigned long long)now() * 1000 + (millis() - msReset) % 1000 -
         msDelay;
}