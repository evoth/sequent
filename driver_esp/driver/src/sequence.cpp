#include "sequence.h"
#include "camera.h"
#include "server.h"
#include "status.h"
#include <ArduinoJson.h>
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

int actionIndex;
int totalActions;
bool isRunning = false;
unsigned long nextTime = 0;
unsigned long startTime = 0;
File file;
JsonDocument action;

char sequenceFilePath[] = "/run.seq";

// Time until next shot in milliseconds (clamped at 0)
unsigned long timeUntilNext() {
  unsigned long currentTime = millis() - startTime;
  if (currentTime >= nextTime) return 0;
  return nextTime - currentTime;
}

void readAction() {
  deserializeJson(action, file);
  isRunning = file.findUntil(",","]");
  float actionStart = action["start"];
  nextTime = actionStart * 1000;
  actionIndex++;
}

void startSequence() {
  if (!SD.begin()) {
        Serial.println("SD card mount failed!");
        return;
  }
  Serial.println("SD card mount done.");

  file = SD.open(sequenceFilePath, FILE_READ);
  if (!file) {
      Serial.println("Failed to open file for reading");
      return;
  }

  file.find("\"numActions\":");
  totalActions = file.parseInt();
  file.find("\"actions\":[");
  actionIndex = -1;
  readAction();

  isRunning = true;
  startTime = millis();
  snprintf(statusMsg, sizeof(statusMsg), "Sequence started successfully.");
}

void stopSequence() {
  if (file) file.close();
  SD.end();
  isRunning = false;
  snprintf(statusMsg, sizeof(statusMsg), "Sequence stopped successfully.");
}

// Run in main loop
void loopSequence() {
  if (!isRunning || timeUntilNext() > 0) return;
  setExposure(action["data"]["states"]["tv"].as<String>().c_str(), action["data"]["states"]["iso"].as<String>().c_str());
  triggerShutter();
  readAction();
  sendStatus();
}