#ifndef SEQUENT_CAMERA_H
#define SEQUENT_CAMERA_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "logger.h"

// TODO: Manage status and state
// TODO: make logger private
class CameraCCAPI {
 public:
  CameraCCAPI() : logger("CCAPI") {}

  char cameraIP[32];
  bool cameraConnected = false;
  Logger logger;

  void connect(const char* ipAddress);
  void triggerShutter();
  void setExposure(const char* tv, const char* iso);

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
};

#endif
