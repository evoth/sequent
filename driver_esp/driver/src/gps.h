#ifndef SEQUENT_GPS_H
#define SEQUENT_GPS_H

#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include "elapsedMillis.h"
#include "logger.h"

class GPS {
 public:
  GPS() : logger("GPS") {
    // Hardcoded as Serial2 for now (GPIO 16 and 17)
    Serial2.begin(9600);
    logger.log("Begin Serial2 at 9600 baud.");
  }

  void read() {
    while (Serial2.available() > 0)
      gps.encode(Serial2.read());
    // If we have newly acquired time data, sync immediately
    if (hasTime() && !prevHasTime)
      syncElapsed = syncInterval + 1;
    prevHasTime = hasTime();
  }
  void getState(JsonObject& stateObject) { logger.getRecentLogs(stateObject); }
  void syncTime();
  bool hasTime() { return gps.date.isValid() && gps.time.isValid(); }
  bool shouldSync() { return syncElapsed > syncInterval; }

 private:
  Logger logger;
  TinyGPSPlus gps;
  const unsigned long long syncInterval = 300000;
  elapsedMillis syncElapsed;
  bool prevHasTime = false;
};

#endif