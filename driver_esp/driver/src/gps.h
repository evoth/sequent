#ifndef SEQUENT_GPS_H
#define SEQUENT_GPS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <elapsedMillis.h>
#include "device.h"

class GPS : public Device {
 public:
#if defined(ESP32)
  GPS() {
    strncpy(logger.name, "GPS @ Serial2", sizeof(logger.name));
    // Hardcoded as Serial2 for now (GPIO 16 and 17)
    Serial2.begin(9600);
    logger.log("Begin Serial2 at 9600 baud.", false, true);
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
#elif defined(ESP8266)
  GPS() {
    strncpy(logger.name, "GPS (not supported)", sizeof(logger.name));
    logger.error("GPS not supported on ESP8266.", false, true);
  }

  void recordGpsData(const char* gpsCsvFilePath) {
    logger.error("GPS not supported on ESP8266.");
  };
#endif

 private:
  TinyGPSPlus gps;
  const unsigned long long syncInterval = 300000;
  elapsedMillis syncElapsed;
  bool prevHasTime = false;

#if defined(ESP32)
  void read();
  void syncTime();
  bool hasTime() { return gps.date.isValid() && gps.time.isValid(); }
#elif defined(ESP8266)
  void read() {}
  void syncTime() { logger.error("GPS not supported on ESP8266."); }
  bool hasTime() { return false; }
#endif
};

#endif