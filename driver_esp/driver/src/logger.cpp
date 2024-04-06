#include "logger.h"

const char* Logger::LOG_FILE = "/logs.txt";
const char* Logger::ERROR_FILE = "/errors.txt";

void Logger::getRecent(const std::vector<std::shared_ptr<Log>>& logs,
                       const JsonObject& logsObject) {
  logsObject["name"] = name;
  JsonArray logsArray = logsObject["logs"].to<JsonArray>();
  for (const auto messageInfo : logs) {
    JsonDocument message;
    message["time"] = messageInfo->time;
    message["message"] = messageInfo->message;
    message["isError"] = messageInfo->isError;
    logsArray.add(message);
  }
}