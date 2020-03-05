#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "Arduino.h"
#include "WateringSchedules.h"

void WateringSchedules::printSchedules() {
  for (int i = 0; i < sizeOfSchedulesWatering; ++i) {
    Serial.print("START => ");
    Serial.print(schedulesWatering[i][0]);
    Serial.print(":");
    Serial.print(schedulesWatering[i][1]);

    Serial.print(" | END => ");
    Serial.print(schedulesWatering[i][2]);
    Serial.print(":");
    Serial.println(schedulesWatering[i][3]);
  }
}

int** WateringSchedules::getSchedules() {
  return (schedulesWatering);
}

int WateringSchedules::getSizeOfSchedulesWatering() {
  return (sizeOfSchedulesWatering);
}

void WateringSchedules::getWateringSchedules() {

  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    HTTPClient http;

    http.begin(endpoint + idWatering); //Specify the URL
    int httpCode = http.GET();  //Make the request

    if (httpCode == 200) { //Check for the returning code
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);

      DynamicJsonBuffer jsonBuffer2;
      JsonObject& root = jsonBuffer2.parseObject(payload);
      JsonArray& requests = root["schedules"];

      const char* id_watering = root["id_watering"];

      sizeOfSchedulesWatering = requests.size();
      schedulesWatering = new int*[sizeOfSchedulesWatering];
      int i = 0;
      for (auto& request : requests) {
        schedulesWatering[i] = new int [4];

        char* start = strdup(request["start"]);
        char* startInt = strtok (start, ":");
        int start_hour = atoi(startInt);
        startInt = strtok (NULL, ":");
        int start_minute = atoi(startInt);

        char* end = strdup(request["end"]);
        char* endInt = strtok (end, ":");
        int end_hour = atoi(endInt);
        endInt = strtok (NULL, ":");
        int end_minute = atoi(endInt);

        schedulesWatering[i][0] = start_hour;
        schedulesWatering[i][1] = start_minute;
        schedulesWatering[i][2] = end_hour;
        schedulesWatering[i][3] = end_minute;
        i++;
      }
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpCode);
    }

    http.end(); //Free the resources
  }
}
