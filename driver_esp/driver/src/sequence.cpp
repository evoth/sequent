#include "sequence.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <optional>
#include "cameraCCAPI.h"
#include "logger.h"
#include "sequentServer.h"

// Time until next shot in milliseconds (clamped at 0)
unsigned long Sequence::timeUntil(unsigned long testTime) {
  unsigned long currentTime = millis() - sequenceStartTime;
  if (currentTime >= testTime)
    return 0;
  return testTime - currentTime;
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
  if (file.peek() == ']' || actionIndex >= totalActions - 1) {
    actionIndex++;
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

  strncpy(filePath, sequenceFilePath, sizeof(filePath));
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
  sequenceStartTime = millis();
  logger.log("Sequence '%s' started.", filePath);
}

void Sequence::stop() {
  vector<tuple<unsigned long, StateManagerInterface*, int>>::iterator it;
  for (it = endQueue.begin(); it != endQueue.end(); it = endQueue.erase(it)) {
    get<1>(*it)->removeState(get<2>(*it));
  }
  isRunning = false;
  logger.log("Sequence '%s' stopped.", filePath);
}

// Run in main loop
// TODO: Clean this up... I'm in a hurry :)
bool Sequence::loop() {
  if (isRunning && endQueue.empty() && actionIndex >= totalActions) {
    logger.log("Sequence '%s' completed.", filePath);
    stop();
    return true;
  }

  if (isRunning) {
    vector<tuple<unsigned long, StateManagerInterface*, int>>::iterator it;
    for (it = endQueue.begin(); it != endQueue.end();) {
      if (timeUntil(get<0>(*it)) > 0) {
        it++;
      } else {
        get<1>(*it)->removeState(get<2>(*it));
        it = endQueue.erase(it);
      }
    }
  }

  if (!isRunning || timeUntil(nextTime) > 0 || actionIndex >= totalActions)
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
  } else if (actionId == "photo" || actionId == "video") {
    String ipString = action["data"]["states"]["ip"];
    if (cameras.count(ipString) == 0) {
      logger.error("Camera with IP address %s has not been connected.",
                   ipString.c_str());
    } else {
      Camera* camera = cameras[ipString];
      camera->startAction(action["layer"], action["data"]);
      float actionEnd = action["end"];
      endQueue.push_back(make_tuple((unsigned long)actionEnd * 1000, camera,
                                    action["layer"].as<int>()));
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