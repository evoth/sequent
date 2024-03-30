#ifndef SEQUENT_CAMERA_CCAPI_H
#define SEQUENT_CAMERA_CCAPI_H

#include <HTTPClient.h>
#include <WiFi.h>
#include "camera.h"
#include "logger.h"

class CameraCCAPI : public Camera {
 public:
  CameraCCAPI(const char* ipAddress) : Camera(ipAddress) {
    snprintf(logger.name, sizeof(logger.name), "CCAPI Camera @ %s", ipAddress);
  }

  void connect();
  void triggerShutter();
  void setIso(const char* iso) {
    setValueAPI("/ver100/shooting/settings/iso", iso, "ISO");
  }
  void setAv(const char* av) {
    setValueAPI("/ver100/shooting/settings/av", av, "aperture");
  }
  void setTv(const char* tv) {
    setValueAPI("/ver100/shooting/settings/tv", tv, "shutter speed");
  }
  void startRecording() {
    actionAPI("/ver100/shooting/control/recbutton", "start",
              "Started recording.");
  }
  void stopRecording() {
    actionAPI("/ver100/shooting/control/recbutton", "stop",
              "Stopped recording.");
  }
  void movieModeOn() {
    actionAPI("/ver100/shooting/control/moviemode", "on",
              "Entered movie mode.");
  }
  void movieModeOff() {
    actionAPI("/ver100/shooting/control/moviemode", "off",
              "Exited movie mode.");
  }

 private:
  char apiUrl[64];
  const char* apiUrlTemplate = "http://%s:8080/ccapi";
  WiFiClient client;
  HTTPClient http;

  // Figure out how to make capture of statusCode optional in respective lambdas
  void request(const char* url,
               std::function<int()> action,
               std::function<void(int statusCode)> success,
               std::function<void(int statusCode)> failure);
  void setValueAPI(const char* path, const char* val, const char* name);
  void actionAPI(const char* path, const char* val, const char* message);
};

#endif
