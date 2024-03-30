#ifndef SEQUENT_CAMERA_PTPIP_H
#define SEQUENT_CAMERA_PTPIP_H

#include <WiFi.h>
#include "camera.h"
#include "logger.h"

class CameraPTPIP : public Camera {
 public:
  CameraPTPIP(const char* ipAddress) : Camera(ipAddress) {
    snprintf(logger.name, sizeof(logger.name), "PTP/IP Camera @ %s", ipAddress);
  }

  void connect();
  void triggerShutter(){};
  void setIso(const char* iso) {}
  void setAv(const char* av) {}
  void setTv(const char* tv) {}
  void startRecording() {}
  void stopRecording() {}
  void movieModeOn() {}
  void movieModeOff() {}

 private:
  WiFiClient commandClient;
  WiFiClient eventClient;

  bool readResponse(WiFiClient& client, char* buffer, size_t size);
};

#endif
