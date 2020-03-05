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

private:
    MValveActuator valve;
    
    void _clearChannel() {
      if (GLOBAL_INSTANCE)
        GLOBAL_INSTANCE->setValue("");
    }

    const std::unordered_map<std::string, std::function<void(std::string arg)>> _commandScheme = {
        { "start", [&](std::string&& arg) { start(std::move(arg)); } },
        { "stop", [&](std::string&& arg) { stop(std::move(arg)); } },
        { "schedules", [&](std::string&& arg) { schedules(std::move(arg)); } },
        { "sensor", [&](std::string&& arg) { sensor(std::move(arg)); } },
        { "schedules_and_sensor", [&](std::string&& arg) { schedulesSensor(std::move(arg)); } },
    };
};
