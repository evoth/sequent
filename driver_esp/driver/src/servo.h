#ifndef SEQUENT_SERVO_H
#define SEQUENT_SERVO_H

#include <ServoEasing.h>
#include "device.h"
#include "state.h"

struct ServoState {
  int angle = 0;
  int speed = 0;
};

class SequentServo : public StateManager<ServoState>, public Device {
 public:
  SequentServo(int servoPin) : servoPin(servoPin) {
    snprintf(logger.name, sizeof(logger.name), "Servo @ pin %d", servoPin);
  }

  void begin(int initialAngle) {
    servo.attach(servoPin, initialAngle, 544, 2400);
    // TODO: Allow selection of easing type in action
    servo.setEasingType(EASE_CUBIC_IN_OUT);
    // pinMode(servoPin, INPUT);
    logger.log("Attached to pin %d.", servoPin);
  };

 protected:
  void actOnDiff(ServoState& oldState,
                 ServoState& newState,
                 bool fromDefault = false);
  ServoState stateFromAction(int layer, const JsonObject& actionData);

 private:
  int servoPin;
  ServoEasing servo;
};

#endif