#include "deviceManager.h"
#include "cameraCCAPI.h"
#include "cameraPTPIP.h"

void DeviceManager::getStatus(const JsonArray& statesArray) {
  JsonDocument doc;
  JsonObject gpsStatus = doc.to<JsonObject>();
  gps.getStatus(gpsStatus);
  statesArray.add(gpsStatus);
  for (auto& [ip, camera] : cameras) {
    JsonDocument doc;
    JsonObject cameraStatus = doc.to<JsonObject>();
    camera->getStatus(cameraStatus);
    statesArray.add(cameraStatus);
  }
}

shared_ptr<StateManagerInterface> DeviceManager::processAction(
    Logger& logger,
    JsonDocument& action) {
  shared_ptr<StateManagerInterface> actionDevice(nullptr);

  String actionId = action["data"]["action"];

  // Dispatch actions to relevant objects
  if (actionId == "connect") {
    String ipString = action["data"]["states"]["ip"];
    if (cameras.count(ipString) == 0) {
      String method = action["data"]["states"]["method"];
      if (method == "CCAPI") {
        cameras[ipString] =
            shared_ptr<Camera>(new CameraCCAPI(ipString.c_str()));
      } else if (method == "PTP/IP") {
        cameras[ipString] =
            shared_ptr<Camera>(new CameraPTPIP(ipString.c_str()));
      }
    }
    cameras[ipString]->connect();
  } else if (actionId == "photo" || actionId == "video") {
    String ipString = action["data"]["states"]["ip"];
    if (cameras.count(ipString) == 0) {
      logger.error("Camera with IP address %s has not been connected.",
                   ipString.c_str());
    } else {
      shared_ptr<Camera> camera = cameras[ipString];
      camera->startAction(action["layer"], action["data"]);
      // TODO: Do this for all actions by using a StateManager smart pointer
      actionDevice = shared_ptr<StateManagerInterface>(camera);
    }
  }

  return actionDevice;
}