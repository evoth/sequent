#ifndef SEQUENT_SERVER_H
#define SEQUENT_SERVER_H

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <WebSocketsServer.h>
#if defined(ESP32)
#include <WiFi.h>
#define ESP_NAME "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#define ESP_NAME "ESP8266"
#endif
#include <map>
#include <memory>
#include "deviceManager.h"
#include "sequence.h"

class SequentServer {
 public:
  SequentServer() : server(80), webSocket(81), devices(new DeviceManager()) {
    const char* macAddress = WiFi.macAddress().c_str();
    snprintf(serverId, sizeof(serverId), "%c%c%c%c%c%c", macAddress[9],
             macAddress[10], macAddress[12], macAddress[13], macAddress[15],
             macAddress[16]);
    snprintf(logger.name, sizeof(logger.name), "Server %s", serverId);
  }

  void init() {
    char ssid[32];
    // snprintf(ssid, sizeof(ssid), "%s_AP_%s", ESP_NAME, serverId);
    snprintf(ssid, sizeof(ssid), "%s_AP_14", ESP_NAME, serverId);
    initAP(ssid, "aaaaaaaa");
    initWebServer();
    initWebSocketServer();
  }
  void loop();

 private:
  AsyncWebServer server;
  WebSocketsServer webSocket;
  JsonDocument msg;
  bool newMsg = false;
  int msgClient;
  std::map<String, std::shared_ptr<Sequence>> sequences;
  Logger logger;
  std::shared_ptr<DeviceManager> devices;
  char serverId[7];
  File uploadFile;
  bool shouldSendStatus = false;
  char jsonBuffer[4096];

  void initAP(const char* ssid, const char* password);
  void initWebServer();
  void initWebSocketServer();
  void sendStatus();
  void webSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t* payload,
                      size_t length);
};

#endif
