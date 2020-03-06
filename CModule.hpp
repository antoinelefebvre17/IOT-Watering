#pragma once

#include <unordered_map>
#include <functional>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

BLECharacteristic *GLOBAL_INSTANCE = nullptr;



enum MODE_T {
  NONE = 0,
  START,
  STOP,
  SCHEDULES,
  SENSOR,
  SCHEDULESSENSOR
} MODE;

class MValveActuator {
  public:
    MValveActuator() = default;
    ~MValveActuator() = default;

    static void turnOn() {
      digitalWrite(LED_BUILTIN, HIGH);
    }

    static void turnOff() {
      digitalWrite(LED_BUILTIN, LOW);
    }

    static void turn() {
      if (digitalRead(LED_BUILTIN) == LOW) {
        digitalWrite(LED_BUILTIN, HIGH);
      } else if (digitalRead(LED_BUILTIN) == HIGH) {
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
};

class CModule {
  public:
    CModule() {}
    ~CModule() = default;

    template<typename... Args>
    bool interpret(std::string&& str, std::string&& arg = "") {
      if (this->_commandScheme.find(str) != this->_commandScheme.end()) {
        this->_commandScheme.at(str)(arg);
        _clearChannel();
        return true;
      }
      _clearChannel();
      return false;
    }

    void init() {
      wateringSchedules.getWateringSchedules();
      wateringSchedules.printSchedules();
    
      timeLocal.setup();
    
      sensorDHT.setup();
    }

    void start(std::string&& = "") {
      Serial.println("MODE START");
      MODE = START;
    }

    void stop(std::string&& = "") {
      Serial.println("MODE STOP");
      MODE = STOP;
    }


    void schedules(std::string&& = "") {
      Serial.println("MODE SCHEDULES ONLY");
      MODE = SCHEDULES;
    }

    void sensor(std::string&& = "") {
      Serial.println("MODE SENSOR ONLY");
      MODE = SENSOR;
    }

    void schedulesSensor(std::string&& = "") {
      Serial.println("MODE SCHEDULES AND SENSOR");
      MODE = SCHEDULESSENSOR;
    }

    void process() {
      processDHSensor();
      
      switch (MODE) {
        case START: MValveActuator::turnOn(); Serial.println("Mode START");
          break;
        case STOP: MValveActuator::turnOff(); Serial.println("Mode STOP");
          break;
        case SCHEDULES: Serial.println("Mode SCHEDULES");
          if (checkActivateWateringByOnlySchedules() == true) {
            MValveActuator::turnOn();
          } else {
            MValveActuator::turnOff();
          }
          break;
        case SENSOR: Serial.println("Mode SENSOR");
          if (checkActivateWateringByOnlyDHT() == true) {
            MValveActuator::turnOn();
          } else {
            MValveActuator::turnOff();
          }
          break;
        case SCHEDULESSENSOR: Serial.println("Mode SCHEDULESSENSOR");
          if (checkActivateWateringByDHTAndSchedules() == true) {
            MValveActuator::turnOn();
          } else {
            MValveActuator::turnOff();
          }
          break;
        case NONE: Serial.println("Mode NONE");
        default: break;
      }
       Serial.println();
    }

  private:
    MValveActuator valve;
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
      int maxHumidity = 50;
    
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
    
    void _clearChannel() {
      if (GLOBAL_INSTANCE)
        GLOBAL_INSTANCE->setValue("");
    }

    void processDHSensor() {
        sensorDHT.start();
    }

    const std::unordered_map<std::string, std::function<void(std::string arg)>> _commandScheme = {
      { "start", [&](std::string&& arg) {
          start(std::move(arg));
        }
      },
      { "stop", [&](std::string&& arg) {
          stop(std::move(arg));
        }
      },
      { "schedules", [&](std::string&& arg) {
          schedules(std::move(arg));
        }
      },
      { "sensor", [&](std::string&& arg) {
          sensor(std::move(arg));
        }
      },
      { "schedules_and_sensor", [&](std::string&& arg) {
          schedulesSensor(std::move(arg));
        }
      },
    };
};
