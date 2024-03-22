#ifndef SEQUENT_SEQUENCE_H
#define SEQUENT_SEQUENCE_H

#include <ArduinoJson.h>
#include <map>
#include "cameraCCAPI.h"
#include "logger.h"

// TODO: Make camera private
// TODO: make logger private
// Generalize for any number of cameras
class Sequence {
 public:
  Sequence() : logger("Sequence") {}

  int actionIndex = 0;
  int totalActions = 0;
  bool isRunning = false;
  const char* filePath;
  std::map<String, Camera*> cameras;
  Logger logger;

  unsigned long timeUntilNext();
  void readAction();
  void start(const char* sequenceFilePath);
  void stop();
  bool loop();
  void getCamerasStatus(const JsonArray& camerasArray);

 private:
  unsigned long startTime = 0;
  unsigned long nextTime = 0;
  JsonDocument action;
  unsigned long filePos;
};

#endif
