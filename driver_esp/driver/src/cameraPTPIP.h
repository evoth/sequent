#ifndef SEQUENT_CAMERA_PTPIP_H
#define SEQUENT_CAMERA_PTPIP_H

#include <WiFi.h>
#include <elapsedMillis.h>
#include <map>
#include "camera.h"
#include "logger.h"

class CameraPTPIP : public Camera {
 public:
  CameraPTPIP(const char* ipAddress) : Camera(ipAddress) {
    snprintf(logger.name, sizeof(logger.name), "PTP/IP Camera @ %s", ipAddress);
  }

  void connect();

 protected:
  void triggerShutter();
  void setIso(const char* iso) {
    setPropertyValueWrapper(ISO_CODE, ISO_MAP, iso, "ISO");
  }
  void setAv(const char* av) {
    setPropertyValueWrapper(AV_CODE, AV_MAP, av, "aperture");
  }
  void setTv(const char* tv) {
    setPropertyValueWrapper(TV_CODE, TV_MAP, tv, "shutter speed");
  }
  void startRecording() {}
  void stopRecording() {}
  void movieModeOn() {}
  void movieModeOff() {}

 private:
  WiFiClient commandClient;
  WiFiClient eventClient;
  uint32_t transactionId = 0;
  const unsigned long long pollInterval = 200;
  elapsedMillis pollElapsed;

  const uint32_t AV_CODE = 0xd101;
  const uint32_t TV_CODE = 0xd102;
  const uint32_t ISO_CODE = 0xd103;

  // TODO: Find additional mappings using Wireshark
  // https://github.com/JulianSchroden/cine_remote/blob/ab6709f1144673755afc3243c53a02304fe5a1cb/lib/camera_control/eos_ptp_ip/constants/ptp_property.dart#L36
  std::map<String, uint32_t> AV_MAP = {
      {"f1.8", 0x15},  {"f2.0", 0x18},  {"f2.2", 0x1b},  {"f2.5", 0x1d},
      {"f2.8", 0x20},  {"f3.2", 0x23},  {"f3.5", 0x25},  {"f4.0", 0x28},
      {"f4.5", 0x2b},  {"f5.0", 0x2d},  {"f5.6", 0x30},  {"f6.3", 0x33},
      {"f7.1", 0x35},  {"f8.0", 0x38},  {"f9.0", 0x3b},  {"f10.0", 0x3d},
      {"f11.0", 0x40}, {"f13.0", 0x43}, {"f14.0", 0x45}, {"f16.0", 0x48},
      {"f16.0", 0x48},
  };
  std::map<String, uint32_t> TV_MAP = {
      {"30\"", 0x10},   {"25\"", 0x13},   {"20\"", 0x15},   {"15\"", 0x18},
      {"13\"", 0x1b},   {"10\"", 0x1d},   {"8\"", 0x20},    {"6\"", 0x23},
      {"5\"", 0x25},    {"4\"", 0x28},    {"3\"2", 0x2b},   {"2\"5", 0x2d},
      {"2\"", 0x30},    {"1\"6", 0x33},   {"1\"3", 0x35},   {"1\"", 0x38},
      {"0\"8", 0x3b},   {"0\"6", 0x3d},   {"0\"5", 0x40},   {"0\"4", 0x43},
      {"0\"3", 0x45},   {"1/4", 0x48},    {"1/5", 0x4b},    {"1/6", 0x4d},
      {"1/8", 0x50},    {"1/10", 0x53},   {"1/13", 0x55},   {"1/15", 0x58},
      {"1/20", 0x5b},   {"1/25", 0x5d},   {"1/30", 0x60},   {"1/40", 0x63},
      {"1/50", 0x65},   {"1/60", 0x68},   {"1/80", 0x6b},   {"1/100", 0x6d},
      {"1/125", 0x70},  {"1/160", 0x73},  {"1/200", 0x75},  {"1/250", 0x78},
      {"1/320", 0x7b},  {"1/400", 0x7d},  {"1/500", 0x80},  {"1/640", 0x83},
      {"1/800", 0x85},  {"1/1000", 0x88}, {"1/1250", 0x8b}, {"1/1600", 0x8d},
      {"1/2000", 0x90}, {"1/2500", 0x93}, {"1/3200", 0x95}, {"1/4000", 0x98},
      {"1/5000", 0x9b}, {"1/6400", 0x9d}, {"1/8000", 0xa0},
  };
  std::map<String, uint32_t> ISO_MAP = {
      {"auto", 0x00}, {"100", 0x48},   {"125", 0x4b},   {"160", 0x4d},
      {"200", 0x50},  {"250", 0x53},   {"320", 0x55},   {"400", 0x58},
      {"500", 0x5b},  {"640", 0x5d},   {"800", 0x60},   {"1000", 0x63},
      {"1250", 0x65}, {"1600", 0x68},  {"2000", 0x6b},  {"2500", 0x6d},
      {"3200", 0x70}, {"4000", 0x73},  {"5000", 0x75},  {"6400", 0x78},
      {"8000", 0x7b}, {"10000", 0x7d}, {"12800", 0x80},
  };

  bool readResponse(WiFiClient& client, char* buffer, size_t size);
  uint32_t getTransactionId() { return transactionId++; }
  bool setPropertyValue(uint32_t propertyCode, uint32_t propertyValue);
  void setPropertyValueWrapper(uint32_t propCode,
                               std::map<String, uint32_t> valMap,
                               const char* val,
                               const char* name) {
    String valString = val;
    if (valMap.count(valString) == 0)
      logger.error("Unsupported %s value: %s", name, val);

    if (setPropertyValue(propCode, valMap[valString]))
      logger.log("Set %s to %s", name, val);
  }
  void pollEvents();
};

#endif
