#ifndef SEQUENT_DEVICE_H
#define SEQUENT_DEVICE_H

#include <ArduinoJson.h>
#include <map>
#include <memory>
#include <optional>
#include "device.h"
#include "logger.h"

// TODO: Make further generalizations by taking advantage of this class
class Device {
 public:
  virtual bool loop() { return false; }
  virtual void getStatus(JsonObject& stateObject) {
    logger.getRecentLogs(stateObject);
  }

 protected:
  Logger logger;
};

#endif
