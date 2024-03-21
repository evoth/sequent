#include "server.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "camera.h"
#include "resources.h"
#include "sequence.h"
#include "status.h"

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
JsonDocument msg;
bool newMsg = false;
Sequence sequence;

void initAP() {
  const char* ssid = "ESP32_AP";
  const char* password = "defgecd7";

  Serial.print("Starting soft-AP... ");
  WiFi.softAP(ssid, password);

  Serial.print("Soft-AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void initWebServer() {
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
}

void sendStatus() {
  JsonDocument status;
  status["statusCode"] = statusCode;
  status["statusMsg"] = statusMsg;
  status["cameraConnected"] = cameraConnected;
  status["cameraIP"] = cameraIP;
  status["isRunning"] = sequence.isRunning;
  status["actionIndex"] = sequence.actionIndex;
  status["totalActions"] = sequence.totalActions;
  status["timeUntilNext"] = sequence.timeUntilNext();
  String statusText;
  serializeJson(status, statusText);
  webSocket.broadcastTXT(statusText);
}

void webSocketEvent(uint8_t num,
                    WStype_t type,
                    uint8_t* payload,
                    size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0],
                    ip[1], ip[2], ip[3], payload);
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

void initWebSocketServer() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void initServer() {
  initAP();
  initWebServer();
  initWebSocketServer();
}

// Execute command based on most recent WebSocket message
void loopServer() {
  sequence.loop();

  webSocket.loop();
  if (!newMsg)
    return;

  String command = msg["command"];

  if (command == "connect") {
    cameraConnect(msg);
  } else if (command == "start") {
    sequence.start("/run.seq");
  } else if (command == "stop") {
    sequence.stop();
  } else {
    statusCode = 0;
    snprintf(statusMsg, sizeof(statusMsg), "Unknown command.");
  }

  sendStatus();
  newMsg = false;
}