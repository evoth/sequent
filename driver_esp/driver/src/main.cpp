#include "server.h"
#include "sequence.h"
#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  Serial.println();
  initServer();
}

void loop()
{
  loopServer();
  loopSequence();
}