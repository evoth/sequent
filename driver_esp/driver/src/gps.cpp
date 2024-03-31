#include "gps.h"
#include "timeMillis.h"

void GPS::syncTime() {
  read();
  syncElapsed = 0;
  if (gps.date.isValid() && gps.time.isValid()) {
    if (gps.time.age() > 1000) {
      logger.error("Stale data; time not updated.");
      return;
    }

    setTime(gps.time.hour(), gps.time.minute(), gps.time.second(),
            gps.date.day(), gps.date.month(), gps.date.year());
    msOffset = millis() - (gps.time.centisecond() * 10) - gps.time.age();

    logger.log("Synced time @ %lu ms.", millis());
  } else {
    logger.error("Invalid date/time; time not updated.");
  }
}

void GPS::read() {
  while (Serial2.available() > 0)
    gps.encode(Serial2.read());
  // If we have newly acquired time data, sync immediately
  if (hasTime() && !prevHasTime)
    syncElapsed = syncInterval + 1;
  prevHasTime = hasTime();
}