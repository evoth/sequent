#ifndef SEQUENT_BME_SENSOR_H
#define SEQUENT_BME_SENSOR_H

#include <Adafruit_BME280.h>
#include "device.h"

class BMESensor : public Device {
 public:
  BMESensor() { strncpy(logger.name, "BMESensor @ I2C", sizeof(logger.name)); }

  void recordSensorData(const char* sensorCsvFilePath);

 private:
  Adafruit_BME280 bme;
  bool isConnected = false;
  bool begin();
};

#endif