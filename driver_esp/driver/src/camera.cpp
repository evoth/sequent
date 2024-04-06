#include "camera.h"

CameraState Camera::stateFromAction(int layer, const JsonObject& actionData) {
  CameraState state;

  String actionId = actionData["action"];
  if (actionId == "photo" || actionId == "exposure") {
    state.mode = actionData["states"]["mode"].as<String>();

    // Only take photo if the new state will be on top (and if it's a photo
    // action)
    if (actionId == "photo" && layer >= getTopIndex()) {
      state.takePhoto = true;
    }

    // Start by setting all properties, unless auto
    if (state.mode != "auto") {
      state.tv = actionData["states"]["tv"].as<String>();
      state.av = actionData["states"]["av"].as<String>();
      state.iso = actionData["states"]["iso"].as<String>();
      state.ev = actionData["states"]["ev"].as<String>();
    }

    // Unset properties as appropriate
    if (state.mode == "m")
      state.ev.reset();
    if (state.mode == "av" || state.mode == "p")
      state.tv.reset();
    if (state.mode == "tv" || state.mode == "p")
      state.av.reset();

  } else if (actionId == "video") {
    state.mode = "movie";
    state.isRecording = true;
  } else if (actionId == "displayOnOff") {
    state.isDisplayOn = actionData["states"]["action"].as<String>() == "on";
  } else if (actionId == "shutterRelease") {
    state.shutterPressed = true;
  }

  return state;
}

// TODO: Don't propagate state unless action is successful
void Camera::actOnDiff(CameraState& oldState,
                       CameraState& newState,
                       bool fromDefault) {
  // TODO: Turn this repeated code into a function which takes a lambda for what
  // to do with changed value

  if (!newState.isDisplayOn.has_value())
    newState.isDisplayOn = oldState.isDisplayOn;
  if (newState.isDisplayOn != oldState.isDisplayOn) {
    if (newState.isDisplayOn.value())
      displayOn();
    else
      displayOff();
  }
  if (newState.isDisplayOn.has_value())
    oldState.isDisplayOn = newState.isDisplayOn;

  if (newState.isRecording != oldState.isRecording) {
    if (newState.isRecording) {
      movieModeOn();
      startRecording();
    } else {
      stopRecording();
      movieModeOff();
    }
  }

  if (!newState.mode.has_value())
    newState.mode = oldState.mode;
  if (newState.mode != oldState.mode && newState.mode.has_value()) {
    // TODO: Implement mode switching
    // So it turns out my camera can't switch photography modes via WiFi, only
    // between photo and movie mode :/
    // This is scuffed but it 5:03am
    if (newState.mode == "movie") {
      // movieModeOn();
    } else {
      movieModeOff();
    }
  }
  if (fromDefault && newState.mode.has_value())
    oldState.mode = newState.mode;

  if (!newState.tv.has_value())
    newState.tv = oldState.tv;
  if (newState.tv != oldState.tv)
    // TODO: Implement bulb exposures
    setTv(newState.tv.value().c_str());
  if (fromDefault && newState.tv.has_value())
    oldState.tv = newState.tv;

  if (!newState.av.has_value())
    newState.av = oldState.av;
  if (newState.av != oldState.av)
    setAv(newState.av.value().c_str());
  if (fromDefault && newState.av.has_value())
    oldState.av = newState.av;

  if (!newState.iso.has_value())
    newState.iso = oldState.iso;
  if (newState.iso != oldState.iso)
    setIso(newState.iso.value().c_str());
  if (fromDefault && newState.iso.has_value())
    oldState.iso = newState.iso;

  if (!newState.ev.has_value())
    newState.ev = oldState.ev;
  if (newState.ev != oldState.ev) {
    // TODO: Implement EV setting
  }
  if (fromDefault && newState.ev.has_value())
    oldState.ev = newState.ev;

  if (newState.takePhoto) {
    triggerShutter();
    newState.takePhoto = false;
  }

  if (newState.shutterPressed != oldState.shutterPressed) {
    if (newState.shutterPressed)
      shutterDown();
    else
      shutterUp();
  }
}