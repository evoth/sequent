#ifndef SEQUENT_GPS_H
#define SEQUENT_GPS_H

#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <elapsedMillis.h>
#include "logger.h"

class GPS {
 public:
  GPS() : logger("GPS") {
    // Hardcoded as Serial2 for now (GPIO 16 and 17)
    Serial2.begin(9600);
    logger.log("Begin Serial2 at 9600 baud.");
  }

  void getState(JsonObject& stateObject) { logger.getRecentLogs(stateObject); }
  bool loop() {
    read();
    if (syncElapsed > syncInterval) {
      syncTime();
      return true;
    }
    return false;
  }

 private:
  Logger logger;
  TinyGPSPlus gps;
  const unsigned long long syncInterval = 300000;
  elapsedMillis syncElapsed;
  bool prevHasTime = false;

  void read();
  void syncTime();
  bool hasTime() { return gps.date.isValid() && gps.time.isValid(); }
};

#endif