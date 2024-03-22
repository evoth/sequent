#include "sequentServer.h"
#include <WiFi.h>
#include "cameraCCAPI.h"
#include "resources.h"

void SequentServer::initAP(const char* ssid, const char* password) {
  logger.log("Starting soft-AP... ");
  WiFi.softAP(ssid, password);

  logger.log("Soft-AP IP address: %s", WiFi.softAPIP().toString());
}

void SequentServer::initWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "text/html", indexHtml);
  });

  server.onNotFound([](AsyncWebServerRequest* req) {
    if (req->method() == HTTP_OPTIONS) {
      // CORS OPTIONS request
      req->send(200);
    } else {
      req->send(404, "text/plain", "Not found");
    }
  });

  // CORS OPTIONS request
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials",
                                       "true");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods",
                                       "GET,HEAD,OPTIONS,POST,PUT");
  DefaultHeaders::Instance().addHeader(
      "Access-Control-Allow-Headers",
      "Access-Control-Allow-Headers, Origin, Accept, X-Requested-With, "
      "Content-Type, Access-Control-Request-Method, "
      "Access-Control-Request-Headers");

  server.begin();
  logger.log("Started web server.");
}

void SequentServer::initWebSocketServer() {
  webSocket.begin();
  webSocket.onEvent(
      [this](uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
        webSocketEvent(num, type, payload, length);
      });
  logger.log("Started WebSocket server.");
}

// TODO: separate status of different components
void SequentServer::sendStatus() {
  JsonDocument status;
  logger.getRecentLogs(status["serverLogs"].to<JsonArray>());
  sequence.logger.getRecentLogs(status["sequenceLogs"].to<JsonArray>());
  sequence.cameraCCAPI.logger.getRecentLogs(
      status["cameraLogs"].to<JsonArray>());
  status["cameraConnected"] = sequence.cameraCCAPI.cameraConnected;
  status["cameraIP"] = sequence.cameraCCAPI.cameraIP;
  status["isRunning"] = sequence.isRunning;
  status["actionIndex"] = sequence.actionIndex;
  status["totalActions"] = sequence.totalActions;
  status["timeUntilNext"] = sequence.timeUntilNext();
  char statusText[4096];
  serializeJson(status, statusText);
  webSocket.broadcastTXT(statusText);
}

void SequentServer::webSocketEvent(uint8_t num,
                                   WStype_t type,
                                   uint8_t* payload,
                                   size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      logger.log("[%u] Disconnected!", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      logger.log("[%u] Connected from %d.%d.%d.%d url: %s", num, ip[0], ip[1],
                 ip[2], ip[3], payload);
      sendStatus();
    } break;
    case WStype_TEXT:
      newMsg = true;
      deserializeJson(msg, (const char*)payload);
      break;
    default:
      break;
  }
}

// Execute command based on most recent WebSocket message
void SequentServer::loop() {
  if (sequence.loop())
    sendStatus();

  webSocket.loop();
  if (!newMsg)
    return;

  String command = msg["command"];

  if (command == "connect") {
    sequence.cameraCCAPI.connect(msg["cameraIP"]);
  } else if (command == "start") {
    sequence.start("/run.seq");
  } else if (command == "stop") {
    sequence.stop();
  } else {
    logger.error("Unknown command: %s.", command);
  }

  sendStatus();
  newMsg = false;
}