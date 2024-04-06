#include "shutterRelease.h"

ShutterState ShutterRelease::stateFromAction(int layer,
                                             const JsonObject& actionData) {
  ShutterState state;

  String actionId = actionData["action"];
  if (actionId == "shutterRelease") {
    state.shutterPressed = true;
  }

  return state;
}

void ShutterRelease::actOnDiff(ShutterState& oldState,
                               ShutterState& newState,
                               bool fromDefault) {
  if (newState.shutterPressed != oldState.shutterPressed) {
    if (newState.shutterPressed) {
      logger.log("Pin %d HIGH to press shutter.", shutterPin);
      digitalWrite(shutterPin, HIGH);
    } else {
      logger.log("Pin %d LOW to release shutter.", shutterPin);
      digitalWrite(shutterPin, LOW);
    }
  }
}