#include <Arduino.h>
#include "sequence.h"
#include "server.h"

void setup() {
  Serial.begin(115200);
  Serial.println();
  initServer();
}

void loop() {
  loopServer();
}