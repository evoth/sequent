#include <Arduino.h>
#include <SD.h>
#include <ServoEasing.hpp>
#include "sequence.h"
#include "sequentServer.h"

SequentServer server;

void setup() {
  Serial.begin(115200);
  Serial.println();

  if (!SD.begin()) {
    Serial.println("SD card mount failed!");
    return;
  }
  server.init("ESP32_AP_%s", "defgecd7");
}

void loop() {
  server.loop();
}