#include "sequentServer.h"
#include <SD.h>
#include <WiFi.h>
#include "cameraCCAPI.h"
#include "resources.h"
#include "timeMillis.h"

void SequentServer::initAP(const char* ssid, const char* password) {
  logger.log("Starting soft-AP... ");
  WiFi.softAP(ssid, password);

  logger.log("Soft-AP IP address: %s", WiFi.softAPIP().toString());
}

void SequentServer::initWebServer() {
  // Driver web client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", indexHtml);
  });

  server.on("/icon.svg", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "image/svg+xml", iconSvg);
  });

  // List of .seq files in root directory of SD card
  server.on("/seq-files", HTTP_GET, [](AsyncWebServerRequest* request) {
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
    request->send(200, "application/json", filenamesTxt);
  });

  // Upload new sequence file
  server.on(
      "/upload", HTTP_POST,
      [this](AsyncWebServerRequest* request) {
        uploadFile.close();
        request->send(200);
      },
      [this](AsyncWebServerRequest* request, String filename, size_t index,
             uint8_t* data, size_t len, bool final) {
        String filePath = "/" + filename;
        if (!index) {
          logger.log("Starting upload of file '%s'", filePath.c_str());
          uploadFile = SD.open(filePath, FILE_WRITE);
          if (!uploadFile) {
            logger.error("Failed to open file '%s' for writing.",
                         filePath.c_str());
            return;
          }
        }
        if (uploadFile) {
          if (uploadFile.write(data, len) != len) {
            uploadFile.close();
            logger.error("Failed to write to file '%s'.", filePath.c_str());
          }
        }
        if (final) {
          uploadFile.close();
          logger.log("Completed upload of %u bytes to file '%s'.", index + len,
                     filePath.c_str());
        }
      });

  // Restart ESP
  server.on("/reboot", HTTP_POST, [this](AsyncWebServerRequest* request) {
    logger.log("Rebooting...");
    ESP.restart();
  });

  // Delete sequence file
  server.on("/delete", HTTP_DELETE, [this](AsyncWebServerRequest* request) {
    shouldSendStatus = true;

    if (!request->hasArg("file")) {
      logger.error("Request to /delete without required parameter 'file'.");
      request->send(400, "text/plain",
                    "Request to /delete without required parameter 'file'.");
      return;
    }

    const char* filePath = request->arg("file").c_str();
    logger.log("Deleting file '%s'.", filePath);

    if (!SD.exists(filePath)) {
      logger.error("File '%s' doesn't exist.", filePath);
      request->send(200, "File doesn't exist.");
      return;
    }

    if (!SD.remove(filePath)) {
      logger.error("Failed to delete '%s'.", filePath);
      request->send(200, "Failed to delete file.");
      return;
    }

    logger.log("Deleted '%s'.", filePath);
    request->send(200);
  });

  server.onNotFound([](AsyncWebServerRequest* request) {
    if (request->method() == HTTP_OPTIONS) {
      // CORS OPTIONS request
      request->send(200);
    } else {
      request->send(404, "text/plain", "Not found");
    }
  });

  // CORS OPTIONS request
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials",
                                       "true");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods",
                                       "GET,HEAD,OPTIONS,POST,PUT,DELETE");
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

// Construct JsonDocument for represent state, then serialize and broadcast it
// to WebSocket clients
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
  devices.getStatus(states);
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
      msgClient = num;
      deserializeJson(msg, (const char*)payload);
      break;
    default:
      break;
  }
}

// Execute command based on most recent WebSocket message
void SequentServer::loop() {
  if (sequence.loop() || shouldSendStatus) {
    sendStatus();
    shouldSendStatus = false;
  }

  webSocket.loop();
  if (!newMsg)
    return;

  String command = msg["command"];

  if (command == "start") {
    sequence.start(msg["filename"]);
  } else if (command == "stop") {
    sequence.stop();
  } else if (command == "setTime") {
    unsigned long long timeMs = msg["time"];
    setTime(timeMs / 1000);
    msReset = millis() - timeMs % 1000;
    msDelay = 0;
    logger.log("Synced time from client %d @ %lu ms.", msgClient, millis());
  } else {
    logger.error("Unknown command: %s.", command);
  }

  sendStatus();
  newMsg = false;
}