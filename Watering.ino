#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>

#include "TimeLocal.h"
#include "WifiESP.h"

// pin connected to DH22 data line
#define DATA_PIN 14

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);

class HttpRequest {
  const String endpoint = "https://4ab8a3ec.ngrok.io/wateringManagement/schedules/";
  const String idWatering = "5e5596e321bba050aabba187";

  const size_t capacity = JSON_ARRAY_SIZE(1440) + 1441*JSON_OBJECT_SIZE(3) + 433;
  StaticJsonBuffer<2000> jsonBuffer;

  time_t theTime = time(NULL);
  struct tm *aTime = localtime(&theTime);

  int **schedulesWatering;
  int sizeOfSchedulesWatering;

  public:
  void printSchedules() {
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

  int **getSchedules() {
    return (schedulesWatering);
  }

  int getSizeOfSchedulesWatering() {
    return (sizeOfSchedulesWatering);
  }
  
  void getWateringSchedules() {
    
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
          char* startInt = strtok (start,":");
          int start_hour = atoi(startInt);
          startInt = strtok (NULL,":");
          int start_minute = atoi(startInt);
          
          char* end = strdup(request["end"]);
          char* endInt = strtok (end,":");
          int end_hour = atoi(endInt);
          endInt = strtok (NULL,":");
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
};

class SensorDHT {
  float celsius;
  float humidity;

  public:
  void setup() {
    dht.begin();
  }

  void start() {
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    
    celsius = event.temperature;

    dht.humidity().getEvent(&event);

    humidity = event.relative_humidity;

    Serial.print("celsius: ");
    Serial.print(celsius);
    Serial.println("C");

    Serial.print("humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  float getCelsius() {
    return (celsius);
  }

  float getHumidity() {
    return (humidity);
  }
};

TimeLocal timeLocal;
HttpRequest httpRequest;
SensorDHT sensorDHT;

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  WifiESP wifiESP;

  wifiESP.setup();
  httpRequest.getWateringSchedules();
  httpRequest.printSchedules();

  timeLocal.setup();

  sensorDHT.setup();
  // put your setup code here, to run once:

}

bool checkActivateWateringByOnlySchedules() {
  int **schedulesWatering = httpRequest.getSchedules();

    for (int i = 0; i < httpRequest.getSizeOfSchedulesWatering(); ++i) {
      if (timeLocal.getHour() >= schedulesWatering[i][0] && timeLocal.getHour() <= schedulesWatering[i][2]) {
        if (timeLocal.getMinutes() >= schedulesWatering[i][1] && timeLocal.getMinutes() < schedulesWatering[i][3]) {
          //Serial.print("Local hour => ");
          //Serial.println(timeLocal.getHour());
          //Serial.print("Local minutes => ");
          //Serial.println(timeLocal.getMinutes());
          //Serial.print("Schedules start hour => ");
          //Serial.println(schedulesWatering[i][0]);
          //Serial.print("Schedules start minutes => ");
          //Serial.println(schedulesWatering[i][1]);
          //Serial.print("Schedules end hour => ");
          //Serial.println(schedulesWatering[i][2]);
          //Serial.print("Schedules end minutes => ");
          //Serial.println(schedulesWatering[i][3]);
          return (true);
        }
      }
    }
    return (false);
}

bool checkActivateWateringByOnlyDHT() {
  float celsius = sensorDHT.getCelsius();
  float humidity = sensorDHT.getHumidity(); 
  float minCelsius = 20.00;
  float maxCelsius = 40.00;
  float minHumidity = 35.00;
  float maxHumidity = 45.00;

    if (celsius >= minCelsius && celsius < maxCelsius) {
      if (humidity >= minHumidity && humidity < maxHumidity) {
        return (true);
      }
    }
  return (false);
}

bool checkActivateWateringByDHTAndSchedules() {
  if (checkActivateWateringByOnlyDHT() == true && checkActivateWateringByOnlySchedules() == true) {
    return (true);
  } else {
    return (false);
  }
}

void start_watering() {
  if (digitalRead(LED_BUILTIN) == LOW) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void end_watering() {
  if (digitalRead(LED_BUILTIN) == HIGH) {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  
  sensorDHT.start();
  timeLocal.printLocalTime();
  
  if (checkActivateWateringByDHTAndSchedules() == true) {
    Serial.println("ON");
    start_watering();
  } else {
    Serial.println("OFF");
    end_watering();
  }
  
  Serial.println();

}
