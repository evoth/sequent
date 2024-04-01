#include "sequence.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <optional>
#include "cameraCCAPI.h"
#include "cameraPTPIP.h"
#include "logger.h"
#include "sequentServer.h"
#include "timeMillis.h"

// Time until next shot in milliseconds (clamped at 0)
unsigned long long Sequence::timeUntil(unsigned long long testTime) {
  unsigned long long currentTime = fullTimeMs(isAbsolute);
  if (!isAbsolute)
    currentTime -= sequenceStartTime;
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
  // Seek to previous file position (closed between function calls)
  file.seek(filePos);

  // Check for start of next JSON object
  // TODO: make this robust against whitespace
  if (file.peek() == ',')
    file.find(",");
  // Check for end of JSON array
  if (file.peek() == ']' || actionIndex >= totalActions - 1) {
    actionIndex++;
    return;
  };

  // We're always one action "ahead" after deserializing and action.
  // We set start time, then wait until it's time to execute it.
  deserializeJson(action, file);
  nextTime = action["start"];
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

  // Manually retrieve values from beginning of JSON. If out of order, this will
  // break.
  file.find("\"numActions\":");
  totalActions = file.parseInt();
  file.find("\"isAbsolute\":");
  isAbsolute = file.read() == 't';
  file.find("\"actions\":[");
  filePos = file.position();
  file.close();

  // Read first action
  actionIndex = -1;
  readAction();
  isRunning = true;
  sequenceStartTime = fullTimeMs(isAbsolute);
  logger.log("Sequence '%s' started.", filePath);
}

void Sequence::stop() {
  vector<tuple<unsigned long long, shared_ptr<StateManagerInterface>,
               int>>::iterator it;
  for (it = endQueue.begin(); it != endQueue.end(); it = endQueue.erase(it)) {
    get<1>(*it)->removeState(get<2>(*it));
  }
  isRunning = false;
  logger.log("Sequence '%s' stopped.", filePath);
}

// Run in main loop
// TODO: Clean this up... I'm in a hurry :)
bool Sequence::loop() {
  bool shouldSendState = devices->loop();

  // Sequence is done (only after all actions have finished)
  if (isRunning && endQueue.empty() && actionIndex >= totalActions) {
    logger.log("Sequence '%s' completed.", filePath);
    stop();
    return true;
  }

  // Removes states that have ended, which triggers any necessary actions
  if (isRunning) {
    vector<tuple<unsigned long long, shared_ptr<StateManagerInterface>,
                 int>>::iterator it;
    for (it = endQueue.begin(); it != endQueue.end();) {
      if (timeUntil(get<0>(*it)) > 0) {
        it++;
      } else {
        get<1>(*it)->removeState(get<2>(*it));
        it = endQueue.erase(it);
      }
    }
  }

  // Nothing to do
  if (!isRunning || timeUntil(nextTime) > 0 || actionIndex >= totalActions)
    return shouldSendState;

  // Skip action because we're already past the end
  if (timeUntil(action["end"]) == 0) {
    readAction();
    return shouldSendState;
  }

  logger.log("Starting action %d", actionIndex);
  // logger.log("Min free heap size: %d", esp_get_minimum_free_heap_size());

  shared_ptr<StateManagerInterface> actionDevice =
      devices->processAction(logger, action);
  if (actionDevice != nullptr)
    endQueue.push_back(
        make_tuple(action["end"], actionDevice, action["layer"]));

  readAction();
  return true;
}