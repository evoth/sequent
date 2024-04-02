#ifndef SEQUENT_GPS_H
#define SEQUENT_GPS_H

#include <TinyGPSPlus.h>
#include <elapsedMillis.h>
#include "device.h"

class GPS : public Device {
 public:
  GPS() {
    strncpy(logger.name, "GPS @ Serial2", sizeof(logger.name));
    // Hardcoded as Serial2 for now (GPIO 16 and 17)
    Serial2.begin(9600);
    logger.log("Begin Serial2 at 9600 baud.");
  }

  bool loop() {
    read();
    if (syncElapsed > syncInterval) {
      syncTime();
      return true;
    }
    return false;
  }

  void recordGpsData(const char* gpsCsvFilePath);

 private:
  TinyGPSPlus gps;
  const unsigned long long syncInterval = 300000;
  elapsedMillis syncElapsed;
  bool prevHasTime = false;

  void read();
  void syncTime();
  bool hasTime() { return gps.date.isValid() && gps.time.isValid(); }
};

#endif