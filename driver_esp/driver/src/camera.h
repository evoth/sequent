#ifndef ESP_INT_CAMERA_H
#define ESP_INT_CAMERA_H

#include <ArduinoJson.h>

extern char cameraIP[32];
extern bool cameraConnected;
extern bool bulbMode;

extern void cameraConnect(JsonDocument doc);
extern void triggerShutter();
extern void setExposure(const char *tv, const char *iso);

#endif
