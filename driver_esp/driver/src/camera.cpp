#include "camera.h"

CameraState Camera::stateFromAction(int layer, const JsonObject& actionData) {
  CameraState state;

  String actionId = actionData["action"];
  if (actionId == "photo") {
    state.mode = actionData["states"]["mode"].as<String>();

    if (layer >= getTopIndex()) {
      state.takePhoto = true;
    }

    if (state.mode != "auto") {
      state.tv = actionData["states"]["tv"].as<String>();
      state.av = actionData["states"]["av"].as<String>();
      state.iso = actionData["states"]["iso"].as<String>();
      state.ev = actionData["states"]["ev"].as<String>();
    }

    if (state.mode == "m")
      state.ev.reset();
    if (state.mode == "av" || state.mode == "p")
      state.tv.reset();
    if (state.mode == "tv" || state.mode == "p")
      state.av.reset();
  }

  return state;
}

void Camera::actOnDiff(CameraState& oldState, CameraState& newState) {
  // TODO: Turn this repeated code into a function which takes a lambda for what
  // to do with changed value

  if (!newState.mode.has_value())
    newState.mode = oldState.mode;
  if (newState.mode != oldState.mode) {
    // TODO: Implement mode switching
  }

  if (!newState.tv.has_value())
    newState.tv = oldState.tv;
  if (newState.tv != oldState.tv)
    // TODO: Implement bulb exposures
    setTv(newState.tv.value().c_str());

  if (!newState.av.has_value())
    newState.av = oldState.av;
  if (newState.av != oldState.av)
    setAv(newState.av.value().c_str());

  if (!newState.iso.has_value())
    newState.iso = oldState.iso;
  if (newState.iso != oldState.iso)
    setIso(newState.iso.value().c_str());

  if (!newState.ev.has_value())
    newState.ev = oldState.ev;
  if (newState.ev != oldState.ev) {
    // Implement EV setting
  }

  if (newState.takePhoto) {
    triggerShutter();
    newState.takePhoto = false;
  }

  // TODO: Implement video recording
}