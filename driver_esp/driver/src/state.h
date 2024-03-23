#ifndef SEQUENT_STATE_H
#define SEQUENT_STATE_H

#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <optional>
#include <vector>
#include "logger.h"
using namespace std;

template <class T>
class StateManager {
 public:
  void startAction(int layer, const JsonObject& actionData) {
    addState(layer, stateFromAction(actionData));
  };
  void endAction(int layer) { removeState(layer); }
  virtual T stateFromAction(const JsonObject& actionData) = 0;
  void getState(JsonObject& stateObject) { logger.getRecentLogs(stateObject); }

 protected:
  Logger logger;
  virtual void actOnDiff(T& a, T& b) = 0;

 private:
  T defaultState;
  vector<T> stateLayers;
  vector<bool> used;

  void addState(int layer, T state);
  void removeState(int layer);
  int getTopIndex() {
    int index = used.size() - 1;
    while (index >= 0 && !used[index])
      index--;
    return index;
  };
};

#endif