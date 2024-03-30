#include "timeMillis.h"

unsigned long msOffset = 0;

unsigned long millisecond() {
  return (millis() - msOffset) % 1000;
}

unsigned long long fullTimeMs(bool useFull) {
  if (!useFull)
    return millis();
  return (unsigned long long)now() * 1000 + millisecond();
}