#ifndef SEQUENT_CAMERA_PTPIP_H
#define SEQUENT_CAMERA_PTPIP_H

#include <WiFi.h>
#include <elapsedMillis.h>
#include "camera.h"
#include "logger.h"

class CameraPTPIP : public Camera {
 public:
  CameraPTPIP(const char* ipAddress) : Camera(ipAddress) {
    snprintf(logger.name, sizeof(logger.name), "PTP/IP Camera @ %s", ipAddress);
  }

  void connect();

 protected:
  void triggerShutter();
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
  uint32_t transactionId = 0;
  const unsigned long long keepAliveInterval = 1000;
  elapsedMillis keepAliveElapsed;

  bool readResponse(WiFiClient& client, char* buffer, size_t size);
  uint32_t getTransactionId() { return transactionId++; }
};

#endif
