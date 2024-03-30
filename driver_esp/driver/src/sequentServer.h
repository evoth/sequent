#ifndef SEQUENT_SERVER_H
#define SEQUENT_SERVER_H

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "sequence.h"

class SequentServer {
 public:
  SequentServer() : server(80), webSocket(81), logger("Server") {}

  void init(const char* ssid, const char* password) {
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
