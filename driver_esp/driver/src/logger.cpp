#include "logger.h"
#include <SD.h>
#include <TimeLib.h>
#include <tuple>
#include "timeMillis.h"

const char* Logger::LOG_FILE = "/logs.txt";
const char* Logger::ERROR_FILE = "/errors.txt";

void Logger::generalLog(int statusCode,
                        const char* format,
                        va_list args,
                        const char* logFilePath,
                        std::vector<std::shared_ptr<Log>>& logs,
                        bool isError) {
  // Make format string based on va_list args captured from caller function
  char msgBuffer[256];
  vsnprintf(msgBuffer, sizeof(msgBuffer), format, args);

  // We keep a vector of recent logs with max length of NUM_RECENT
  unsigned long long currentTime = fullTimeMs(true);
  logs.push_back(std::shared_ptr<Log>(
      new Log(currentTime, msgBuffer, isError, statusCode)));
  if (logs.size() > NUM_RECENT) {
    logs.erase(logs.begin());
  }

  File logFile = SD.open(logFilePath, FILE_APPEND);
  if (!logFile) {
    Serial.printf("Failed to open file '%s' for writing.\n", logFilePath);
    return;
  }

  // TODO: Stop it from printing to serial twice on error
  char logBuffer[256];
  snprintf(logBuffer, sizeof(logBuffer), "[%llu, %d, %d] %s: %s\n", currentTime,
           statusCode, isError, name, msgBuffer);
  Serial.print(logBuffer);
  logFile.print(logBuffer);

  logFile.close();
}

void Logger::getRecent(const std::vector<std::shared_ptr<Log>>& logs,
                       const JsonObject& logsObject) {
  logsObject["name"] = name;
  JsonArray logsArray = logsObject["logs"].to<JsonArray>();
  for (const auto messageInfo : logs) {
    JsonDocument message;
    message["time"] = messageInfo->time;
    message["message"] = messageInfo->message;
    message["isError"] = messageInfo->isError;
    message["statusCode"] = messageInfo->statusCode;
    logsArray.add(message);
  }
}