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

  do {
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

    // Keep reading until action end time is after current time
  } while (timeUntil(action["end"]) == 0);

  filePos = file.position();
  file.close();
}

void Sequence::start() {
  if (isRunning)
    stop();

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

  // Read first action(s) (seek to current time if needed)
  actionIndex = -1;
  isRunning = true;
  sequenceStartTime = fullTimeMs(isAbsolute);
  readAction();
  logger.log("Sequence '%s' started.", filePath);
}

void Sequence::stop() {
  std::vector<std::tuple<unsigned long long,
                         std::shared_ptr<StateManagerInterface>, int>>::iterator
      it;
  for (it = endQueue.begin(); it != endQueue.end(); it = endQueue.erase(it)) {
    std::get<1>(*it)->removeState(std::get<2>(*it));
  }
  isRunning = false;
  logger.log("Sequence '%s' stopped.", filePath);
}

// Run in main loop
// TODO: Clean this up... I'm in a hurry :)
bool Sequence::loop() {
  bool shouldSendStatus = devices->loop();

  // Sequence is done (only after all actions have finished)
  if (isRunning && endQueue.empty() && actionIndex >= totalActions) {
    logger.log("Sequence '%s' completed.", filePath);
    stop();
    return true;
  }

  // Removes states that have ended, which triggers any necessary actions
  if (isRunning) {
    std::vector<
        std::tuple<unsigned long long, std::shared_ptr<StateManagerInterface>,
                   int>>::iterator it;
    for (it = endQueue.begin(); it != endQueue.end();) {
      if (timeUntil(std::get<0>(*it)) > 0) {
        it++;
      } else {
        std::get<1>(*it)->removeState(std::get<2>(*it));
        it = endQueue.erase(it);
      }
    }
  }

  // Nothing to do
  if (!isRunning || timeUntil(nextTime) > 0 || actionIndex >= totalActions)
    return shouldSendStatus;

  logger.log("Starting action %d", actionIndex);
  logger.log("Free heap size: %d", ESP.getFreeHeap());

  std::shared_ptr<StateManagerInterface> actionDevice =
      devices->processAction(logger, action);
  if (actionDevice != nullptr)
    endQueue.push_back(
        std::make_tuple(action["end"], actionDevice, action["layer"]));

  readAction();
  return true;
}