#ifndef SEQUENT_CAMERA_H
#define SEQUENT_CAMERA_H

#include <ArduinoJson.h>
#include <optional>
#include "device.h"
#include "state.h"

struct CameraState {
  std::optional<String> mode;
  bool takePhoto = false;
  std::optional<String> tv;
  std::optional<String> av;
  std::optional<String> iso;
  std::optional<String> ev;
  bool isRecording = false;
  std::optional<bool> isDisplayOn;
  bool shutterPressed = false;
};

class Camera : public StateManager<CameraState>, public Device {
 public:
  Camera(const char* ipAddress) {
    strncpy(cameraIP, ipAddress, sizeof(cameraIP));
    snprintf(logger.name, sizeof(logger.name), "Camera @ %s", ipAddress);
  }

  // TODO: Retrieve camera name/model so it can be used in status/log messages
  virtual void connect() = 0;

 protected:
  char cameraIP[32];
  bool cameraConnected = false;

  void actOnDiff(CameraState& oldState,
                 CameraState& newState,
                 bool fromDefault = false);
  CameraState stateFromAction(int layer, const JsonObject& actionData);

  virtual void triggerShutter() { notImplemented(); }
  virtual void setIso(const char* iso) { notImplemented(); }
  virtual void setAv(const char* av) { notImplemented(); }
  virtual void setTv(const char* tv) { notImplemented(); }
  virtual void startRecording() { notImplemented(); }
  virtual void stopRecording() { notImplemented(); }
  virtual void movieModeOn() { notImplemented(); }
  virtual void movieModeOff() { notImplemented(); }
  virtual void displayOn() { notImplemented(); }
  virtual void displayOff() { notImplemented(); }
  virtual void shutterDown() { notImplemented(); }
  virtual void shutterUp() { notImplemented(); }

 private:
  void notImplemented() { logger.error("Action not implemented."); }
};

#endif
