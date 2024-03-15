#include "camera.h"
#include "status.h"
#include <cstring>
#include <WiFi.h>
#include <HTTPClient.h>

WiFiClient client;
HTTPClient http;

char cameraIP[32];
char apiUrl[64];
bool cameraConnected = false;
const char *apiUrlTemplate = "http://%s:8080/ccapi";

// Helper function that sets up HTTP connection before running the "action" function
// (e.g. http.GET(), http.POST(), etc.), then runs either the success or failure
// functions depending on the returned status code.
void request(const char *url, std::function<int()> action, std::function<void()> success, std::function<void()> failure) {
  // Indicates semantic error like malformed URL
  if (!http.begin(client, url)) {
    snprintf(statusMsg, sizeof(statusMsg), "Could not connect to %s", url);
    statusCode = HTTPC_ERROR_CONNECTION_REFUSED;
    failure();
    return;
  }

  int httpCode = action();
  statusCode = httpCode;

  // HTTP client error
  if (httpCode < 0) {
    snprintf(statusMsg, sizeof(statusMsg), "%s when connecting to %s", http.errorToString(httpCode).c_str(), url);
    http.end();
    failure();
    return;
  }

  // HTTP failure code
  if (httpCode != HTTP_CODE_OK) {
    if (http.getSize() > 0) {
      String payload = http.getString();
      snprintf(statusMsg, sizeof(statusMsg), "Error payload from %s: %s", url, payload.c_str());
    } else {
      snprintf(statusMsg, sizeof(statusMsg), "Error %d at %s", httpCode, url);
    }
    http.end();
    failure();
    return;
  }

  success();

  http.end();
}

// Sends a GET request to base CCAPI URL to establish connection
void cameraConnect(JsonDocument doc) {
  String cameraIPTemp = doc["cameraIP"];
  snprintf(cameraIP, sizeof(cameraIP), cameraIPTemp.c_str());
  snprintf(apiUrl, sizeof(apiUrl), apiUrlTemplate, cameraIP);

  request(apiUrl,
    []() {
      return http.GET();
    },
    []() {
      snprintf(statusMsg, sizeof(statusMsg), "Successfully connected to camera.");
      cameraConnected = true;
    },
    []() {
      cameraConnected = false;
    }
  );
}

// Sends a POST request to trigger the shutter
void triggerShutter() {
  char endpointUrl[128];
  snprintf(endpointUrl, sizeof(endpointUrl), "%s/ver100/shooting/control/shutterbutton", apiUrl);

  request(endpointUrl,
    []() {
      char body[] = "{\"af\": false}";
      return http.POST(body);
    },
    []() {
      snprintf(statusMsg, sizeof(statusMsg), "Successfully triggered shutter.");
    },
    []() {
      if (statusCode < 0) {
        cameraConnected = false;
      }
    }
  );
}

// Set camera exposure to previous setting
void setExposure(const char *tv, const char *iso) {
  int numSuccess = 0;
  char endpointUrl[128];

  snprintf(endpointUrl, sizeof(endpointUrl), "%s/ver100/shooting/settings/tv", apiUrl);
  request(endpointUrl,
    [tv]() {
      JsonDocument body;
      body["value"] = tv;
      String bodyText;
      serializeJson(body, bodyText);
      return http.PUT(bodyText);
    },
    [&numSuccess]() {
      numSuccess++;
    },
    []() {}
  );

  snprintf(endpointUrl, sizeof(endpointUrl), "%s/ver100/shooting/settings/iso", apiUrl);
  request(endpointUrl,
    [iso]() {
      JsonDocument body;
      body["value"] = iso;
      String bodyText;
      serializeJson(body, bodyText);
      return http.PUT(bodyText);
    },
    [&numSuccess]() {
      numSuccess++;
    },
    []() {}
  );
  
  if (numSuccess == 2) {
    snprintf(statusMsg, sizeof(statusMsg), "Successfully set exposure.");
  }
}