#include "sequence.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include "camera.h"
#include "logger.h"
#include "server.h"

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
    isRunning = false;
    logger.log("Sequence '%s' completed.", filePath);
    return;
  };

  deserializeJson(action, file);
  float actionStart = action["start"];
  nextTime = actionStart * 1000;
  actionIndex++;

  filePos = file.position();
  file.close();
}

void Sequence::start(char* sequenceFilePath) {
  if (!SD.begin()) {
    Serial.println("SD card mount failed!");
    return;
  }
  logger.log("SD card mounted.", filePath);

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
  logger.log("Sequence '%s' stopped.", filePath);
  isRunning = false;
  SD.end();
}

// Run in main loop
void Sequence::loop() {
  if (!isRunning || timeUntilNext() > 0)
    return;
  logger.log("Starting action %d", actionIndex);
  setExposure(action["data"]["states"]["tv"].as<String>().c_str(),
              action["data"]["states"]["iso"].as<String>().c_str());
  triggerShutter();
  readAction();
  sendStatus();
}