#ifndef SEQUENT_DEVICE_MANAGER_H
#define SEQUENT_DEVICE_MANAGER_H

#include <map>
#include <memory>
#include "bmeSensor.h"
#include "camera.h"
#include "device.h"
#include "gps.h"
#include "servo.h"
#include "state.h"

class DeviceManager {
 public:
  bool loop() {
    bool shouldSendStatus = false;
    shouldSendStatus = shouldSendStatus || gps.loop();
    for (auto& [ip, camera] : cameras) {
      shouldSendStatus = shouldSendStatus || camera->loop();
    }
    return shouldSendStatus;
  }
  std::shared_ptr<StateManagerInterface> processAction(Logger& logger,
                                                       JsonDocument& action);
  void getStatus(const JsonArray& statesArray);

 private:
  std::map<String, std::shared_ptr<Camera>> cameras;
  GPS gps;
  std::map<int, std::shared_ptr<SequentServo>> servos;
  BMESensor bme;
};

#endif
