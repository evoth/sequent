#ifndef SEQUENT_LOGGER_H
#define SEQUENT_LOGGER_H

#include <ArduinoJson.h>
#include <SD.h>
#include <TimeLib.h>
#include <memory>
#include <tuple>
#include <vector>
#include "timeMillis.h"
#ifndef FILE_APPEND
#define FILE_APPEND FILE_WRITE
#endif

struct Log {
  Log(unsigned long long time, const char* message, bool isError)
      : time(time), isError(isError) {
    strncpy(this->message, message, sizeof(this->message));
  }
  unsigned long long time;
  char message[256];
  bool isError;
};

class Logger {
 public:
  Logger(const char* name) { strncpy(this->name, name, sizeof(this->name)); }
  Logger() {}
  char name[32];

  static const char* LOG_FILE;
  static const char* ERROR_FILE;

  template <typename... Args>
  void log(const char* format, Args... args) {
    log(true, true, format, args...);
  }
  template <typename... Args>
  void error(const char* format, Args... args) {
    error(true, true, format, args...);
  }
  template <typename... Args>
  void log(bool saveToFile,
           bool printToSerial,
           const char* format,
           Args... args) {
    generalLog(format, LOG_FILE, recentLogs, false, saveToFile, printToSerial,
               args...);
  }
  template <typename... Args>
  void error(bool saveToFile,
             bool printToSerial,
             const char* format,
             Args... args) {
    generalLog(format, ERROR_FILE, recentErrors, true, saveToFile, false,
               args...);
    generalLog(format, LOG_FILE, recentLogs, true, saveToFile, printToSerial,
               args...);
  }
  void getRecentLogs(const JsonObject& logsObject) {
    getRecent(recentLogs, logsObject);
  }
  void getRecentErrors(const JsonObject& errorsObject) {
    getRecent(recentErrors, errorsObject);
  }

 private:
  static const int NUM_RECENT = 1;
  std::vector<std::shared_ptr<Log>> recentLogs;
  std::vector<std::shared_ptr<Log>> recentErrors;

  template <typename... Args>
  void generalLog(const char* format,
                  const char* logFilePath,
                  std::vector<std::shared_ptr<Log>>& logs,
                  bool isError,
                  bool saveToFile,
                  bool printToSerial,
                  Args... args) {
    // Make format string based on va_list args captured from caller function
    char msgBuffer[256];
    snprintf(msgBuffer, sizeof(msgBuffer), format, args...);

    // We keep a vector of recent logs with max length of NUM_RECENT
    unsigned long long currentTime = fullTimeMs(true);
    logs.push_back(
        std::shared_ptr<Log>(new Log(currentTime, msgBuffer, isError)));
    if (logs.size() > NUM_RECENT) {
      logs.erase(logs.begin());
    }

    File logFile;
    if (saveToFile) {
      logFile = SD.open(logFilePath, FILE_APPEND);
      if (!logFile) {
        Serial.printf("Failed to open file '%s' for writing.\n", logFilePath);
        return;
      }
    }

    // TODO: Stop it from printing to serial twice on error
    char logBuffer[256];
    snprintf(logBuffer, sizeof(logBuffer), "[%llu, %d] %s: %s\n", currentTime,
             isError, name, msgBuffer);

    if (printToSerial)
      Serial.print(logBuffer);

    if (saveToFile) {
      logFile.print(logBuffer);
      logFile.close();
    }
  }

  void getRecent(const std::vector<std::shared_ptr<Log>>& logs,
                 const JsonObject& logsObject);
};

#endif