#include "sequentServo.h"

ServoState SequentServo::stateFromAction(int layer,
                                         const JsonObject& actionData) {
  ServoState state;

  String actionId = actionData["action"];
  if (actionId == "servoMove") {
    state.angle = actionData["states"]["servoAngle"].as<int>();
    state.speed = actionData["states"]["servoSpeed"].as<int>();
  }

  return state;
}

void SequentServo::actOnDiff(ServoState& oldState,
                             ServoState& newState,
                             bool fromDefault) {
  if (newState.angle != oldState.angle) {
    logger.log("Moving from %d to %d degrees.", oldState.angle, newState.angle);
    // pinMode(servoPin, OUTPUT);
    servo.startEaseTo(newState.angle, newState.speed);
    // pinMode(servoPin, INPUT);
  }
  if (fromDefault)
    oldState.angle = newState.angle;
}