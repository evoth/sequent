#include "logger.h"
#include <SD.h>
#include <TimeLib.h>
#include <tuple>
using namespace std;

const char* Logger::LOG_FILE = "/logs.txt";
const char* Logger::ERROR_FILE = "/errors.txt";

void Logger::generalLog(const char* format,
                        va_list args,
                        const char* filename,
                        vector<tuple<time_t, const char*>> logs) {
  char msgBuffer[256];
  vsnprintf(msgBuffer, sizeof(msgBuffer), format, args);

  logs.push_back(make_tuple(now(), msgBuffer));
  if (logs.size() > NUM_RECENT) {
    logs.erase(logs.begin());
  }

  File logFile = SD.open(filename, FILE_APPEND);
  if (!logFile) {
    Serial.printf("Failed to open file '%s' for writing.\n", filename);
    return;
  }

  Serial.printf("[%d] %s: %s\n", now(), name, msgBuffer);
  logFile.printf("[%d] %s: %s\n", now(), name, msgBuffer);
  logFile.close();
}

JsonArray Logger::getRecent(vector<tuple<time_t, const char*>> logs) {
  JsonDocument doc;
  JsonArray messages = doc.to<JsonArray>();

  for (tuple<time_t, const char*> messageInfo : logs) {
    JsonDocument message;
    message["time"] = get<0>(messageInfo);
    message["message"] = get<1>(messageInfo);
    messages.add(message);
  }

  return messages;
}