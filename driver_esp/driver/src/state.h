#ifndef SEQUENT_STATE_H
#define SEQUENT_STATE_H

#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <optional>
#include <vector>
#include "logger.h"

class StateManagerInterface {
 public:
  virtual void removeState(int layer) = 0;
};

template <class T>
class StateManager : public StateManagerInterface {
 public:
  void startAction(int layer, const JsonObject& actionData) {
    addState(layer, stateFromAction(layer, actionData));
  };
  void endAction(int layer) { removeState(layer); }

 protected:
  virtual void actOnDiff(T& oldState,
                         T& newState,
                         bool fromDefault = false) = 0;
  virtual T stateFromAction(int layer, const JsonObject& actionData) = 0;
  int getTopIndex() {
    int index = used.size() - 1;
    while (index >= 0 && !used[index])
      index--;
    return index;
  }

 private:
  T defaultState;
  std::vector<T> stateLayers;
  std::vector<bool> used;

  void addState(int layer, T state) {
    int topIndex = getTopIndex();
    if (topIndex <= layer) {
      bool fromDefault = topIndex == -1;
      actOnDiff(fromDefault ? defaultState : stateLayers[topIndex], state,
                fromDefault);
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