#include "sequentServer.h"
#include <SD.h>
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

  server.on("/icon.svg", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "image/svg+xml", iconSvg);
  });

  server.on("/seq-files", HTTP_GET, [](AsyncWebServerRequest* req) {
    JsonDocument seqFiles;
    JsonArray filenames = seqFiles["files"].to<JsonArray>();
    File root = SD.open("/");
    while (true) {
      File file = root.openNextFile();
      if (!file)
        break;
      String filename = file.name();
      if (filename.endsWith(".seq"))
        filenames.add("/" + filename);
      file.close();
    }
    char filenamesTxt[4096];
    serializeJson(seqFiles, filenamesTxt);
    req->send(200, "application/json", filenamesTxt);
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

void SequentServer::sendStatus() {
  JsonDocument status;
  JsonArray states = status["states"].to<JsonArray>();
  // TODO: Make this cleaner and standardize stuff
  JsonDocument stateDoc;
  JsonObject serverState = stateDoc.to<JsonObject>();
  logger.getRecentLogs(serverState);
  states.add(serverState);
  JsonDocument sequenceDoc;
  JsonObject sequenceState = sequenceDoc.to<JsonObject>();
  sequence.logger.getRecentLogs(sequenceState);
  states.add(sequenceState);
  sequence.getStates(states);
  status["isRunning"] = sequence.isRunning;
  status["sequenceFilename"] = sequence.filePath;
  status["actionIndex"] = sequence.actionIndex;
  status["totalActions"] = sequence.totalActions;
  // TODO: Make nextTime private again, add data for endTime, put those in
  // method in sequence
  status["timeUntilNext"] = sequence.timeUntil(sequence.nextTime);
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

  if (command == "start") {
    sequence.start(msg["filename"]);
  } else if (command == "stop") {
    sequence.stop();
  } else {
    logger.error("Unknown command: %s.", command);
  }

  sendStatus();
  newMsg = false;
}