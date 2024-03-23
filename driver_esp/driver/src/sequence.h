#ifndef SEQUENT_SEQUENCE_H
#define SEQUENT_SEQUENCE_H

#include <ArduinoJson.h>
#include <map>
#include <tuple>
#include <vector>
#include "cameraCCAPI.h"
#include "logger.h"
#include "state.h"
using namespace std;

// TODO: make logger private
class Sequence {
 public:
  Sequence() : logger("Sequence") {}

  int actionIndex = 0;
  int totalActions = 0;
  bool isRunning = false;
  char filePath[64];
  Logger logger;
  unsigned long nextTime = 0;

  unsigned long timeUntil(unsigned long testTime);
  void readAction();
  void start(const char* sequenceFilePath);
  void stop();
  bool loop();
  void getStates(const JsonArray& camerasArray);

 private:
  unsigned long sequenceStartTime = 0;
  unsigned long endTime = 0;
  vector<tuple<unsigned long, StateManagerInterface*, int>> endQueue;
  JsonDocument action;
  unsigned long filePos;
  std::map<String, Camera*> cameras;
};

#endif
