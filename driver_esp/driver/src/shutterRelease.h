#ifndef SEQUENT_SHUTTER_RELEASE_H
#define SEQUENT_SHUTTER_RELEASE_H

#include "device.h"
#include "state.h"

struct ShutterState {
  bool shutterPressed = false;
};

class ShutterRelease : public StateManager<ShutterState>, public Device {
 public:
  ShutterRelease(int shutterPin) : shutterPin(shutterPin) {
    snprintf(logger.name, sizeof(logger.name), "Shutter release @ pin %d",
             shutterPin);
    pinMode(shutterPin, OUTPUT);
  }

 protected:
  void actOnDiff(ShutterState& oldState,
                 ShutterState& newState,
                 bool fromDefault = false);
  ShutterState stateFromAction(int layer, const JsonObject& actionData);

 private:
  int shutterPin;
};

#endif