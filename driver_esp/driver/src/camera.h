#ifndef SEQUENT_CAMERA_H
#define SEQUENT_CAMERA_H

#include <ArduinoJson.h>
#include "state.h"

struct CameraState {
  optional<String> mode;
  bool takePhoto = false;
  optional<String> tv;
  optional<String> av;
  optional<String> iso;
  bool isRecording = false;
};

// TODO: Manage status and state
// TODO: make logger private
class Camera : public StateManager<CameraState> {
 public:
  Camera(const char* ipAddress) {
    strncpy(cameraIP, ipAddress, sizeof(cameraIP));
    snprintf(logger.name, sizeof(logger.name), "Camera @ %s", ipAddress);
  }

  char cameraIP[32];
  bool cameraConnected = false;

  CameraState stateFromAction(const JsonObject& actionData);
  virtual void connect() = 0;
  virtual void triggerShutter() = 0;
  virtual void setIso(const char* iso) = 0;
  virtual void setAv(const char* av) = 0;
  virtual void setTv(const char* tv) = 0;

 protected:
  void actOnDiff(CameraState& a, CameraState& b);
};

#endif
