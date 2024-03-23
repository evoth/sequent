#ifndef SEQUENT_CAMERA_CCAPI_H
#define SEQUENT_CAMERA_CCAPI_H

#include <ArduinoJson.h>
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
    setValueAPI("/ver100/shooting/settings/iso", "ISO", iso);
  }
  void setAv(const char* av) {
    setValueAPI("/ver100/shooting/settings/av", "aperture", av);
  }
  void setTv(const char* tv) {
    setValueAPI("/ver100/shooting/settings/tv", "shutter speed", tv);
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
  void setValueAPI(const char* path, const char* name, const char* val);
};

#endif
