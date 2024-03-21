#ifndef SEQUENT_LOGGER_H
#define SEQUENT_LOGGER_H

#include <ArduinoJson.h>
#include <TimeLib.h>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

class Logger {
 public:
  Logger(const char* name) : name(name) {}

  static const char* LOG_FILE;
  static const char* ERROR_FILE;

  void log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    generalLog(format, args, LOG_FILE, recentLogs);
    va_end(args);
  }
  void error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    generalLog(format, args, ERROR_FILE, recentErrors);
    generalLog(format, args, LOG_FILE, recentLogs);
    va_end(args);
  }
  JsonArray getRecentLogs() { return getRecent(recentLogs); }
  JsonArray getRecentErrors() { return getRecent(recentErrors); }
  const char* getMostRecent() { return get<1>(recentLogs.back()); }

 private:
  static const int NUM_RECENT = 10;
  const char* name;
  vector<tuple<time_t, const char*>> recentLogs;
  vector<tuple<time_t, const char*>> recentErrors;

  void generalLog(const char* format,
                  va_list args,
                  const char* filename,
                  vector<tuple<time_t, const char*>> logs);
  JsonArray getRecent(vector<tuple<time_t, const char*>> logs);
};

#endif