#ifndef SEQUENT_SERVER_H
#define SEQUENT_SERVER_H

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include <memory>
#include "deviceManager.h"
#include "sequence.h"

class SequentServer {
 public:
  SequentServer()
      : server(80),
        webSocket(81),
        sequence(std::shared_ptr<DeviceManager>(&devices)) {
    uint8_t macAddress[6];
    esp_read_mac(macAddress, ESP_MAC_WIFI_STA);
    snprintf(serverId, sizeof(serverId), "%02X%02X%02X", macAddress[3],
             macAddress[4], macAddress[5]);
    snprintf(logger.name, sizeof(logger.name), "Server %s", serverId);
  }

  void init(const char* ssidFormat, const char* password) {
    char ssid[32];
    snprintf(ssid, sizeof(ssid), ssidFormat, serverId);
    initAP(ssid, password);
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
  Sequence sequence;
  Logger logger;
  DeviceManager devices;
  char serverId[7];
  File uploadFile;
  bool shouldSendStatus = false;

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
