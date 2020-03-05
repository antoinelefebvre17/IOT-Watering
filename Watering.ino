#include "TimeLocal.h"
#include "WifiESP.h"
#include "WateringSchedules.h"
#include "SensorDHT.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "CModule.hpp"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


BLEServer *pServer = BLEDevice::createServer();
BLEService *pService = pServer->createService(SERVICE_UUID);
BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE
                                      );
CModule c;

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
  int celsius = static_cast<int>(sensorDHT.getCelsius());
  int humidity = static_cast<int>(sensorDHT.getHumidity());
  int minCelsius = 20;
  int maxCelsius = 40;
  int minHumidity = 35;
  int maxHumidity = 45;

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
  Serial.println("Starting BLE work!");


  BLEDevice::init("watering");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");

  pinMode(LED_BUILTIN, OUTPUT);

  WifiESP wifiESP;

  wifiESP.setup();

  wateringSchedules.getWateringSchedules();
  wateringSchedules.printSchedules();

  timeLocal.setup();

  sensorDHT.setup();

}

void loop() {
  delay(1000);

  sensorDHT.start();
  timeLocal.printLocalTime();

  c.interpret(GLOBAL_INSTANCE->getValue());

  if (checkActivateWateringByDHTAndSchedules() == true) {
    Serial.println("ON");
    
    c.start();
  } else {
    Serial.println("OFF");
    c.stop();
  }

  Serial.println();

}
