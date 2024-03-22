#include "logger.h"
#include <SD.h>
#include <TimeLib.h>
#include <tuple>
using namespace std;

const char* Logger::LOG_FILE = "/logs.txt";
const char* Logger::ERROR_FILE = "/errors.txt";

void Logger::generalLog(int statusCode,
                        const char* format,
                        va_list args,
                        const char* filename,
                        vector<Log>& logs,
                        bool isError) {
  char msgBuffer[256];
  vsnprintf(msgBuffer, sizeof(msgBuffer), format, args);

  logs.push_back(Log(now(), msgBuffer, isError, statusCode));
  if (logs.size() > NUM_RECENT) {
    logs.erase(logs.begin());
  }

  File logFile = SD.open(filename, FILE_APPEND);
  if (!logFile) {
    Serial.printf("Failed to open file '%s' for writing.\n", filename);
    return;
  }

  Serial.printf("[%d, %d, %d] %s: %s\n", now(), statusCode, isError, name,
                msgBuffer);
  logFile.printf("[%d, %d, %d] %s: %s\n", now(), statusCode, isError, name,
                 msgBuffer);
  logFile.close();
}

void Logger::getRecent(const vector<Log>& logs, const JsonArray& logsArray) {
  for (auto messageInfo : logs) {
    JsonDocument message;
    message["time"] = messageInfo.time;
    message["message"] = messageInfo.message;
    message["isError"] = messageInfo.isError;
    message["statusCode"] = messageInfo.statusCode;
    logsArray.add(message);
  }
}