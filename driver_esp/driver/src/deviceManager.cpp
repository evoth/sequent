#include "deviceManager.h"
#include "cameraCCAPI.h"
#include "cameraPTPIP.h"

void DeviceManager::getStatus(const JsonArray& statesArray) {
  JsonDocument gpsDoc;
  JsonObject gpsStatus = gpsDoc.to<JsonObject>();
  gps.getStatus(gpsStatus);
  statesArray.add(gpsStatus);

  JsonDocument bmeDoc;
  JsonObject bmeStatus = bmeDoc.to<JsonObject>();
  bme.getStatus(bmeStatus);
  statesArray.add(bmeStatus);

  for (auto& [ip, camera] : cameras) {
    JsonDocument doc;
    JsonObject cameraStatus = doc.to<JsonObject>();
    camera->getStatus(cameraStatus);
    statesArray.add(cameraStatus);
  }

  for (auto& [servoPin, servo] : servos) {
    JsonDocument doc;
    JsonObject servoStatus = doc.to<JsonObject>();
    servo->getStatus(servoStatus);
    statesArray.add(servoStatus);
  }
}

std::shared_ptr<StateManagerInterface> DeviceManager::processAction(
    Logger& logger,
    JsonDocument& action) {
  std::shared_ptr<StateManagerInterface> actionDevice(nullptr);

  String actionId = action["data"]["action"];

  // Dispatch actions to relevant objects
  if (actionId == "connect") {
    String ipString = action["data"]["states"]["ip"];
    if (cameras.count(ipString) == 0) {
      String method = action["data"]["states"]["method"];
      if (method == "CCAPI") {
        cameras[ipString] =
            std::shared_ptr<Camera>(new CameraCCAPI(ipString.c_str()));
      } else if (method == "PTP/IP") {
        cameras[ipString] =
            std::shared_ptr<Camera>(new CameraPTPIP(ipString.c_str()));
      }
    }
    cameras[ipString]->connect();
  } else if (actionId == "photo" || actionId == "video" ||
             actionId == "exposure") {
    String ipString = action["data"]["states"]["ip"];
    if (cameras.count(ipString) == 0) {
      logger.error("Camera with IP address %s has not been connected.",
                   ipString.c_str());
    } else {
      std::shared_ptr<Camera> camera = cameras[ipString];
      camera->startAction(action["layer"], action["data"]);
      actionDevice = std::shared_ptr<StateManagerInterface>(camera);
    }
  } else if (actionId == "servoAttach") {
    int servoPin = action["data"]["states"]["servoPin"];
    if (servos.count(servoPin) == 0) {
      servos[servoPin] =
          std::shared_ptr<SequentServo>(new SequentServo(servoPin));
    }
    servos[servoPin]->begin(action["data"]["states"]["servoAngle"].as<int>());
  } else if (actionId == "servoMove") {
    int servoPin = action["data"]["states"]["servoPin"];
    if (servos.count(servoPin) == 0) {
      logger.error("No servo is attached at pin %d.", servoPin);
    } else {
      std::shared_ptr<SequentServo> servo = servos[servoPin];
      servo->startAction(action["layer"], action["data"]);
      actionDevice = std::shared_ptr<StateManagerInterface>(servo);
    }
  } else if (actionId == "bmeRecord") {
    bme.recordSensorData(action["data"]["states"]["csvFile"]);
  }

  return actionDevice;
}