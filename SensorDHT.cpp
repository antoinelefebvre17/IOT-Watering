  #include <DHT.h>
  #include <DHT_U.h>
  
  #include "Arduino.h"
  #include "SensorDHT.h"
  
  // pin connected to DH22 data line
  #define DATA_PIN 14

  // create DHT22 instance
  DHT_Unified dht(DATA_PIN, DHT22);
  
  void SensorDHT::setup() {
    dht.begin();
  }

  void SensorDHT::start() {
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

  float SensorDHT::getCelsius() {
    return (celsius);
  }

  float SensorDHT::getHumidity() {
    return (humidity);
  }
