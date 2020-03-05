#pragma once

#include <unordered_map>
#include <functional>

BLECharacteristic *GLOBAL_INSTANCE = nullptr;

class MValveActuator {
public:
  MValveActuator() = default;
  ~MValveActuator() = default;

  void turnOn() {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  void turnOff() {
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  void turn() {
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
          return true;
        }
        return false;
    }

    void start(std::string&& = "") {
      valve.turnOn();
      _clearChannel();
    }

    void stop(std::string&& = "") {
      valve.turnOff();
      _clearChannel();
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
    };
};
