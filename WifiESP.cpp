#include <WiFi.h>

#include "Arduino.h"
#include "WifiESP.h"

  void WifiESP::setup() {
      WiFi.begin(ssid, password);
 
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
      }
 
      Serial.println("Connected to the WiFi network");
  }
