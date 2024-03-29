#ifndef SEQUENT_LOGGER_H
#define SEQUENT_LOGGER_H

#include <ArduinoJson.h>
#include <TimeLib.h>
#include <memory>
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
  Logger(const char* name) { strncpy(this->name, name, sizeof(this->name)); }
  Logger() {}
  char name[32] = "\0";

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
  void getRecentLogs(const JsonObject& logsObject) {
    getRecent(recentLogs, logsObject);
  }
  void getRecentErrors(const JsonObject& errorsObject) {
    getRecent(recentErrors, errorsObject);
  }

 private:
  static const int NUM_RECENT = 1;
  vector<shared_ptr<Log>> recentLogs;
  vector<shared_ptr<Log>> recentErrors;

  void generalLog(int statusCode,
                  const char* format,
                  va_list args,
                  const char* filename,
                  vector<shared_ptr<Log>>& logs,
                  bool isError);
  void getRecent(const vector<shared_ptr<Log>>& logs,
                 const JsonObject& logsObject);
};

#endif