#ifndef SEQUENT_CAMERA_H
#define SEQUENT_CAMERA_H

#include <ArduinoJson.h>
#include "device.h"
#include "state.h"
#include <optional>

struct CameraState {
  std::optional<String> mode;
  bool takePhoto = false;
  std::optional<String> tv;
  std::optional<String> av;
  std::optional<String> iso;
  std::optional<String> ev;
  bool isRecording = false;
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

  virtual void triggerShutter() = 0;
  virtual void setIso(const char* iso) = 0;
  virtual void setAv(const char* av) = 0;
  virtual void setTv(const char* tv) = 0;
  virtual void startRecording() = 0;
  virtual void stopRecording() = 0;
  virtual void movieModeOn() = 0;
  virtual void movieModeOff() = 0;
};

#endif
