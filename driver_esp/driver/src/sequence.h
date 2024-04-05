#ifndef SEQUENT_SEQUENCE_H
#define SEQUENT_SEQUENCE_H

#include <ArduinoJson.h>
#include <memory>
#include <tuple>
#include <vector>
#include "deviceManager.h"
#include "state.h"

// TODO: make logger private
class Sequence {
 public:
  Sequence(std::shared_ptr<DeviceManager> devices, const char* sequenceFilePath)
      : devices(devices) {
    snprintf(logger.name, sizeof(logger.name), "Sequence @ %s",
             sequenceFilePath);
    strncpy(filePath, sequenceFilePath, sizeof(filePath));
  }

  char filePath[64];
  Logger logger;

  unsigned long long timeUntil(unsigned long long testTime);
  void readAction();
  void start();
  void stop();
  bool loop();
  void getStatus(JsonObject& stateObject) {
    logger.getRecentLogs(stateObject);
    stateObject["isRunning"] = isRunning;
    stateObject["filePath"] = filePath;
    stateObject["actionIndex"] = actionIndex;
    stateObject["totalActions"] = totalActions;
    stateObject["timeUntilNext"] = timeUntil(nextTime);
  }

 private:
  unsigned long long sequenceStartTime = 0;
  unsigned long long endTime = 0;
  std::vector<std::tuple<unsigned long long,
                         std::shared_ptr<StateManagerInterface>,
                         int>>
      endQueue;
  JsonDocument action;
  unsigned long filePos;
  bool isAbsolute;
  std::shared_ptr<DeviceManager> devices;
  unsigned long long nextTime = 0;
  bool isRunning = false;
  int actionIndex = 0;
  int totalActions = 0;
};

#endif
