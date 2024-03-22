#ifndef SEQUENT_SEQUENCE_H
#define SEQUENT_SEQUENCE_H

#include <ArduinoJson.h>
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
  CameraCCAPI cameraCCAPI;
  Logger logger;

  unsigned long timeUntilNext();
  void readAction();
  void start(const char* sequenceFilePath);
  void stop();
  bool loop();

 private:
  unsigned long startTime = 0;
  unsigned long nextTime = 0;
  JsonDocument action;
  const char* filePath;
  unsigned long filePos;
};

#endif
