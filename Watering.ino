#include "TimeLocal.h"
#include "WifiESP.h"
#include "WateringSchedules.h"
#include "SensorDHT.h"

TimeLocal timeLocal;
WateringSchedules wateringSchedules;
SensorDHT sensorDHT;

bool checkActivateWateringByOnlySchedules() {
  int **schedulesWatering = wateringSchedules.getSchedules();

    for (int i = 0; i < wateringSchedules.getSizeOfSchedulesWatering(); ++i) {
      if (timeLocal.getHour() >= schedulesWatering[i][0] && timeLocal.getHour() <= schedulesWatering[i][2]) {
        if (timeLocal.getMinutes() >= schedulesWatering[i][1] && timeLocal.getMinutes() < schedulesWatering[i][3]) {
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

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  WifiESP wifiESP;

  wifiESP.setup();
  wateringSchedules.getWateringSchedules();
  wateringSchedules.printSchedules();

  timeLocal.setup();

  sensorDHT.setup();

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
