#ifndef SensorDHT_h
#define SensorDHT_h

class SensorDHT {
    float celsius;
    float humidity;

  public:
    void setup();
    void start();
    float getCelsius();
    float getHumidity();
};

#endif
