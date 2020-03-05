#include <DHT.h>

#include "SensorDHT.h"

// pin connected to DH22 data line
#define DHTPIN 14
#define DHTTYPE DHT22

// create DHT22 instance
DHT dht(DHTPIN, DHTTYPE);

void SensorDHT::setup() {
  dht.begin();
}

void SensorDHT::start() {
  celsius = dht.readTemperature();

  humidity = dht.readHumidity();

  Serial.print("celsius: ");
  Serial.print(celsius);
  Serial.println("C");

  Serial.print("humidity: ");
  Serial.print(humidity);
  Serial.println("%");
}

float SensorDHT::getCelsius() {
  return (celsius);
}

float SensorDHT::getHumidity() {
  return (humidity);
}
