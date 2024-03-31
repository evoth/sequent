#ifndef SEQUENT_SEQUENCE_H
#define SEQUENT_SEQUENCE_H

#include <ArduinoJson.h>
#include <map>
#include <memory>
#include <tuple>
#include <vector>
#include "cameraCCAPI.h"
#include "gps.h"
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
  unsigned long long nextTime = 0;

  unsigned long long timeUntil(unsigned long long testTime);
  void readAction();
  void start(const char* sequenceFilePath);
  void stop();
  bool loop();
  void getStates(const JsonArray& statesArray);

 private:
  unsigned long long sequenceStartTime = 0;
  unsigned long long endTime = 0;
  vector<tuple<unsigned long long, shared_ptr<StateManagerInterface>, int>>
      endQueue;
  JsonDocument action;
  unsigned long filePos;
  std::map<String, shared_ptr<Camera>> cameras;
  GPS gps;
  bool isAbsolute;
};

#endif
