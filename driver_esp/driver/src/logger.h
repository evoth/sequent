#ifndef SEQUENT_LOGGER_H
#define SEQUENT_LOGGER_H

#include <ArduinoJson.h>
#include <TimeLib.h>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

struct Log {
  Log(time_t time, const char* message, bool isError, int statusCode = 0)
      : time(time), isError(isError), statusCode(statusCode) {
    strncpy(this->message, message, sizeof(this->message));
  }
  time_t time;
  char message[256];
  bool isError;
  int statusCode;
};

class Logger {
 public:
  Logger(const char* name) : name(name) {}

  static const char* LOG_FILE;
  static const char* ERROR_FILE;

  void log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    generalLog(0, format, args, LOG_FILE, recentLogs, false);
    va_end(args);
  }
  void error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    generalLog(-1, format, args, ERROR_FILE, recentErrors, true);
    generalLog(-1, format, args, LOG_FILE, recentLogs, true);
    va_end(args);
  }
  void log(int statusCode, const char* format, ...) {
    va_list args;
    va_start(args, format);
    generalLog(statusCode, format, args, LOG_FILE, recentLogs, false);
    va_end(args);
  }
  void error(int statusCode, const char* format, ...) {
    va_list args;
    va_start(args, format);
    generalLog(statusCode, format, args, ERROR_FILE, recentErrors, true);
    generalLog(statusCode, format, args, LOG_FILE, recentLogs, true);
    va_end(args);
  }
  void getRecentLogs(const JsonArray& logsArray) {
    getRecent(recentLogs, logsArray);
  }
  void getRecentErrors(const JsonArray& errorsArray) {
    getRecent(recentErrors, errorsArray);
  }

 private:
  static const int NUM_RECENT = 10;
  const char* name;
  vector<Log> recentLogs;
  vector<Log> recentErrors;

  void generalLog(int statusCode,
                  const char* format,
                  va_list args,
                  const char* filename,
                  vector<Log>& logs,
                  bool isError);
  void getRecent(const vector<Log>& logs, const JsonArray& logsArray);
};

#endif