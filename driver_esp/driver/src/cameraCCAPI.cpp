#include "cameraCCAPI.h"
#include <ArduinoJson.h>
#include <cstring>

// Helper function that sets up HTTP connection before running the "action"
// function (e.g. http.GET(), http.POST(), etc.), then runs either the success
// or failure functions depending on the returned status code.
void CameraCCAPI::request(const char* url,
                          std::function<int()> action,
                          std::function<void(int statusCode)> success,
                          std::function<void(int statusCode)> failure) {
  // Indicates semantic error like malformed URL
  if (!http.begin(client, url)) {
    logger.error(HTTPC_ERROR_CONNECTION_REFUSED, "Could not connect to %s",
                 url);
    failure(HTTPC_ERROR_CONNECTION_REFUSED);
    return;
  }

  int httpCode = action();

  // HTTP client error
  if (httpCode < 0) {
    logger.error(httpCode, "%s when connecting to %s",
                 http.errorToString(httpCode).c_str(), url);
    http.end();
    failure(httpCode);
    return;
  }

  // HTTP failure code
  if (httpCode != HTTP_CODE_OK) {
    if (http.getSize() > 0) {
      String payload = http.getString();
      logger.error(httpCode, "Error payload from %s: %s", url, payload.c_str());
    } else {
      logger.error(httpCode, "Error %d at %s", httpCode, url);
    }
    http.end();
    failure(httpCode);
    return;
  }

  success(httpCode);

  http.end();
}

// Sends a GET request to base CCAPI URL to establish connection
void CameraCCAPI::connect() {
  snprintf(apiUrl, sizeof(apiUrl), apiUrlTemplate, cameraIP);

  request(
      apiUrl, [this]() { return http.GET(); },
      [this](int statusCode) {
        logger.log(statusCode, "Connected to camera at %s.", cameraIP);
        cameraConnected = true;
      },
      [this](int statusCode) { cameraConnected = false; });
}

// Sends a POST request to trigger the shutter
void CameraCCAPI::triggerShutter() {
  char endpointUrl[128];
  snprintf(endpointUrl, sizeof(endpointUrl),
           "%s/ver100/shooting/control/shutterbutton", apiUrl);

  request(
      endpointUrl,
      [this]() {
        char body[] = "{\"af\": false}";
        return http.POST(body);
      },
      [this](int statusCode) { logger.log(statusCode, "Triggered shutter."); },
      [this](int statusCode) {
        if (statusCode < 0) {
          cameraConnected = false;
        }
      });
}

// Endpoints expecting PUT request of format {"value": VALUE_STRING}
void CameraCCAPI::setValueAPI(const char* path,
                              const char* val,
                              const char* name) {
  char endpointUrl[128];

  snprintf(endpointUrl, sizeof(endpointUrl), "%s%s", apiUrl, path);
  request(
      endpointUrl,
      [this, val]() {
        JsonDocument body;
        body["value"] = val;
        String bodyText;
        serializeJson(body, bodyText);
        return http.PUT(bodyText);
      },
      [this, name, val](int statusCode) {
        logger.log(statusCode, "Set %s to %s", name, val);
      },
      [this](int statusCode) {
        if (statusCode < 0) {
          cameraConnected = false;
        }
      });
}

// Endpoints expecting POST request of format {"action": ACTION_STRING}
void CameraCCAPI::actionAPI(const char* path,
                            const char* val,
                            const char* message) {
  char endpointUrl[128];

  snprintf(endpointUrl, sizeof(endpointUrl), "%s%s", apiUrl, path);
  request(
      endpointUrl,
      [this, val]() {
        JsonDocument body;
        body["action"] = val;
        String bodyText;
        serializeJson(body, bodyText);
        return http.POST(bodyText);
      },
      [this, message, val](int statusCode) { logger.log(statusCode, message); },
      [this](int statusCode) {
        if (statusCode < 0) {
          cameraConnected = false;
        }
      });
}