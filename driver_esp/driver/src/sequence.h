#ifndef SEQUENT_SEQUENCE_H
#define SEQUENT_SEQUENCE_H

#include <ArduinoJson.h>
#include <memory>
#include <tuple>
#include <vector>
#include "deviceManager.h"
#include "state.h"
using namespace std;

// TODO: make logger private
class Sequence {
 public:
  Sequence(shared_ptr<DeviceManager> devices)
      : logger("Sequence"), devices(devices) {}

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

 private:
  unsigned long long sequenceStartTime = 0;
  unsigned long long endTime = 0;
  vector<tuple<unsigned long long, shared_ptr<StateManagerInterface>, int>>
      endQueue;
  JsonDocument action;
  unsigned long filePos;
  bool isAbsolute;
  shared_ptr<DeviceManager> devices;
};

#endif
