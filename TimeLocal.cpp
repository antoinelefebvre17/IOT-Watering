#include <WiFi.h>

#include "TimeLocal.h"

void TimeLocal::printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "Local time: %H:%M:%S");
}

int TimeLocal::getHour() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (-1);
  }
  return (timeinfo.tm_hour);
}

int TimeLocal::getMinutes() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (-1);
  }
  return (timeinfo.tm_min);
}

void TimeLocal::setup() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
