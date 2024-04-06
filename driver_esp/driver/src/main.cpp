#include <Arduino.h>
#include <SD.h>
#include <ServoEasing.hpp>
#include "sequence.h"
#include "sequentServer.h"

#if defined(ESP32)
#define PIN_SPI_CS 5U
#elif defined(ESP8266)
#define PIN_SPI_CS 15U
#endif

SequentServer server;

void setup() {
  Serial.begin(115200);
  Serial.println();

  if (!SD.begin(PIN_SPI_CS)) {
    Serial.println("SD card mount failed!");
    return;
  }
  server.init();
}

void loop() {
  delay(1);
  server.loop();
}