#include "cameraCCAPI.h"
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
void CameraCCAPI::connect(const char* ipAddress) {
  snprintf(cameraIP, sizeof(cameraIP), ipAddress);
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

// Set camera exposure to previous setting
void CameraCCAPI::setExposure(const char* tv, const char* iso) {
  int numSuccess = 0;
  char endpointUrl[128];

  snprintf(endpointUrl, sizeof(endpointUrl), "%s/ver100/shooting/settings/tv",
           apiUrl);
  request(
      endpointUrl,
      [this, tv]() {
        JsonDocument body;
        body["value"] = tv;
        String bodyText;
        serializeJson(body, bodyText);
        return http.PUT(bodyText);
      },
      [&numSuccess](int statusCode) { numSuccess++; }, [](int statusCode) {});

  snprintf(endpointUrl, sizeof(endpointUrl), "%s/ver100/shooting/settings/iso",
           apiUrl);
  request(
      endpointUrl,
      [this, iso]() {
        JsonDocument body;
        body["value"] = iso;
        String bodyText;
        serializeJson(body, bodyText);
        return http.PUT(bodyText);
      },
      [&numSuccess](int statusCode) { numSuccess++; }, [](int statusCode) {});

  if (numSuccess == 2) {
    logger.log("Set exposure (tv=%s, iso=%s)", tv, iso);
  }
}