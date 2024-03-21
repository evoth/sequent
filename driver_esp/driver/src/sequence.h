#ifndef SEQUENT_SEQUENCE_H
#define SEQUENT_SEQUENCE_H

#include <ArduinoJson.h>
#include "logger.h"

class Sequence {
 public:
  Sequence() : logger("Sequence") {}

  int actionIndex = 0;
  int totalActions = 0;
  bool isRunning = false;

  unsigned long timeUntilNext();
  void readAction();
  void start(char* sequenceFilePath);
  void stop();
  void loop();

 private:
  unsigned long startTime = 0;
  unsigned long nextTime = 0;
  JsonDocument action;
  Logger logger;
  char* filePath;
  unsigned long filePos;
};

#endif
