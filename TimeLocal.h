#ifndef TimeLocal_h
#define TimeLocal_h

class TimeLocal {
    const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = 0;
    const int   daylightOffset_sec = 0;

  public:
    void printLocalTime();
    int getHour();
    int getMinutes();
    void setup();

};

#endif
