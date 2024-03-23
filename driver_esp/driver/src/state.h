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
    addState(layer, stateFromAction(layer, actionData));
  };
  void endAction(int layer) { removeState(layer); }
  void getState(JsonObject& stateObject) { logger.getRecentLogs(stateObject); }

 protected:
  Logger logger;
  virtual void actOnDiff(T& oldState, T& newState) = 0;
  virtual T stateFromAction(int layer, const JsonObject& actionData) = 0;
  int getTopIndex() {
    int index = used.size() - 1;
    while (index >= 0 && !used[index])
      index--;
    return index;
  }

 private:
  T defaultState;
  vector<T> stateLayers;
  vector<bool> used;

  void addState(int layer, T state) {
    int topIndex = getTopIndex();
    if (topIndex <= layer) {
      actOnDiff(topIndex == -1 ? defaultState : stateLayers[topIndex], state);
    }
    if (stateLayers.size() <= layer)
      stateLayers.resize(layer + 1);
    stateLayers[layer] = state;
    if (used.size() <= layer)
      used.resize(layer + 1);
    used[layer] = true;
  }

  void removeState(int layer) {
    int topIndex = getTopIndex();
    if (used.size() <= layer)
      used.resize(layer + 1);
    used[layer] = false;
    int newTopIndex = getTopIndex();
    if (newTopIndex < topIndex) {
      actOnDiff(stateLayers[topIndex],
                newTopIndex == -1 ? defaultState : stateLayers[newTopIndex]);
    }
  }
};

#endif