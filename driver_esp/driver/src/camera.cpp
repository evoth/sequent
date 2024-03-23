#include "camera.h"

CameraState Camera::stateFromAction(const JsonObject& actionData) {
  CameraState state;
  state.tv = actionData["states"]["tv"].as<String>();
  return state;
}

void Camera::actOnDiff(CameraState& a, CameraState& b) {}