#include "bmeSensor.h"
#include <SD.h>
#include "timeMillis.h"

bool BMESensor::begin() {
  // Hardcoded as I2C for now (GPIO 21 and 22)
  if (!bme.begin(0x76)) {
    logger.error("Could not find a valid BME280 sensor, check wiring!");
    isConnected = false;
  } else {
    isConnected = true;
  }
  return isConnected;
}

void BMESensor::recordSensorData(const char* sensorCsvFilePath) {
  if (!isConnected && !begin())
    return;

  bool isNewFile = !SD.exists(sensorCsvFilePath);

  File sensorCsvFile = SD.open(sensorCsvFilePath, FILE_APPEND);
  if (!sensorCsvFile) {
    logger.error("Failed to open file '%s' for writing.\n", sensorCsvFile);
    return;
  }

  if (isNewFile) {
    sensorCsvFile.printf(
        "Time (ms),Temperature (C),Pressure (hPa),Humidity (%)\n");
  }

  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();

  sensorCsvFile.printf("%llu,%.2f,%.2f,%.2f\n", fullTimeMs(true), temperature,
                       pressure, humidity);

  sensorCsvFile.close();

  logger.log("Recorded data to %s (%.2f C, %.2f hPa, %.2f%%)",
             sensorCsvFilePath, temperature, pressure, humidity);
}