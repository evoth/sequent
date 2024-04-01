#ifndef SEQUENT_DEVICE_MANAGER_H
#define SEQUENT_DEVICE_MANAGER_H

#include "camera.h"
#include "device.h"
#include "gps.h"
#include "state.h"

class DeviceManager {
 public:
  bool loop() {
    bool shouldSendState = false;
    shouldSendState = shouldSendState || gps.loop();
    for (auto& [ip, camera] : cameras) {
      shouldSendState = shouldSendState || camera->loop();
    }
    return shouldSendState;
  }
  shared_ptr<StateManagerInterface> processAction(Logger& logger,
                                                  JsonDocument& action);
  void getStatus(const JsonArray& statesArray);

 private:
  std::map<String, shared_ptr<Camera>> cameras;
  GPS gps;
};

#endif
