#include "gps.h"
#include <SD.h>
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
    msReset = millis();
    // TODO: Reinvestigate this bug
    msDelay = 0;  //(gps.time.centisecond() * 10) - gps.time.age();

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

void GPS::recordGpsData(const char* gpsCsvFilePath) {
  read();
  bool isNewFile = !SD.exists(gpsCsvFilePath);

  File gpsCsvFile = SD.open(gpsCsvFilePath, FILE_APPEND);
  if (!gpsCsvFile) {
    logger.error("Failed to open file '%s' for writing.\n", gpsCsvFile);
    return;
  }

  if (isNewFile) {
    gpsCsvFile.printf(
        "Time (ms),Latitude (degrees),Longitude (degrees),Altitude (m),Number "
        "of "
        "satellites, HDOP (100ths)\n");
  }

  gpsCsvFile.printf("%llu,", fullTimeMs(true));

  if (gps.location.isValid()) {
    gpsCsvFile.printf("%.6f,%.6f,", gps.location.lat(), gps.location.lng());
  } else {
    gpsCsvFile.printf(",,");
  }

  if (gps.altitude.isValid()) {
    gpsCsvFile.printf("%.1f,", gps.altitude.meters());
  } else {
    gpsCsvFile.printf(",");
  }

  if (gps.satellites.isValid()) {
    gpsCsvFile.printf("%d,", gps.satellites.value());
  } else {
    gpsCsvFile.printf(",");
  }

  if (gps.hdop.isValid()) {
    gpsCsvFile.printf("%d", gps.hdop.value());
  }

  gpsCsvFile.printf("\n");

  gpsCsvFile.close();

  if (gps.location.isValid()) {
    logger.log("Recorded data to %s (%.6f, %.6f)", gpsCsvFilePath,
               gps.location.lat(), gps.location.lng());
  } else {
    logger.log("Recorded data to %s (invalid location)", gpsCsvFilePath);
  }
}