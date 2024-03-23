#include "sequence.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include "cameraCCAPI.h"
#include "logger.h"
#include "sequentServer.h"

// Time until next shot in milliseconds (clamped at 0)
unsigned long Sequence::timeUntilNext() {
  unsigned long currentTime = millis() - startTime;
  if (currentTime >= nextTime)
    return 0;
  return nextTime - currentTime;
}

void Sequence::readAction() {
  File file = SD.open(filePath, FILE_READ);
  if (!file) {
    logger.error("Failed to open file '%s' for reading.", filePath);
    return;
  }
  file.seek(filePos);

  if (file.peek() == ',')
    file.find(",");
  if (file.peek() == ']') {
    logger.log("Sequence '%s' completed.", filePath);
    stop();
    return;
  };

  deserializeJson(action, file);
  float actionStart = action["start"];
  nextTime = actionStart * 1000;
  actionIndex++;

  filePos = file.position();
  file.close();
}

void Sequence::start(const char* sequenceFilePath) {
  if (isRunning)
    stop();

  filePath = sequenceFilePath;
  File file = SD.open(filePath, FILE_READ);
  if (!file) {
    logger.error("Failed to open file '%s' for reading.", filePath);
    return;
  }

  file.find("\"numActions\":");
  totalActions = file.parseInt();
  file.find("\"actions\":[");
  filePos = file.position();
  file.close();

  actionIndex = -1;
  readAction();

  isRunning = true;
  startTime = millis();
  logger.log("Sequence '%s' started.", filePath);
}

void Sequence::stop() {
  isRunning = false;
  logger.log("Sequence '%s' stopped.", filePath);
}

// Run in main loop
bool Sequence::loop() {
  if (!isRunning || timeUntilNext() > 0)
    return false;
  logger.log("Starting action %d", actionIndex);

  String actionId = action["data"]["action"];

  if (actionId == "connect") {
    String ipString = action["data"]["states"]["ip"];
    if (cameras.count(ipString) == 0) {
      String method = action["data"]["states"]["method"];
      if (method == "CCAPI") {
        cameras[ipString] = new CameraCCAPI(ipString.c_str());
      }
    }
    cameras[ipString]->connect();
  } else if (actionId == "photo") {
    String ipString = action["data"]["states"]["ip"];
    if (cameras.count(ipString) == 0) {
      logger.error("Camera with IP address %s has not been connected.",
                   ipString.c_str());
    } else {
      Camera& camera = *cameras[ipString];
      camera.startAction(action["layer"], action["data"]);
    }
  }

  readAction();
  return true;
}

void Sequence::getStates(const JsonArray& camerasArray) {
  for (auto& [ip, camera] : cameras) {
    JsonDocument doc;
    JsonObject cameraStatus = doc.to<JsonObject>();
    camera->getState(cameraStatus);
    camerasArray.add(cameraStatus);
  }
}