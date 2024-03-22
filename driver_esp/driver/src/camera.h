#ifndef SEQUENT_CAMERA_H
#define SEQUENT_CAMERA_H

#include <ArduinoJson.h>
#include "logger.h"

// TODO: Manage status and state
// TODO: make logger private
class Camera {
 public:
  Camera(const char* ipAddress, const char* loggerName, const char* method)
      : logger(loggerName), method(method) {
    snprintf(cameraIP, sizeof(cameraIP), ipAddress);
  }

  char cameraIP[32];
  bool cameraConnected = false;
  Logger logger;
  const char* method;

  virtual void connect() = 0;
  virtual void triggerShutter() = 0;
  virtual void setIso(const char* iso) = 0;
  virtual void setAv(const char* av) = 0;
  virtual void setTv(const char* tv) = 0;
};

#endif
