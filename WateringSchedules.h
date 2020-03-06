#ifndef WateringSchedules_h
#define WateringSchedules_h

#include <ArduinoJson.h>

const String endpoint = "https://fed89fd0.ngrok.io/wateringManagement/schedules/";
const String idWatering = "5e5596e321bba050aabba187";

class WateringSchedules {
private:
    int **schedulesWatering;
    int sizeOfSchedulesWatering;

  public:
    void printSchedules();
    int **getSchedules();
    int getSizeOfSchedulesWatering();
    void getWateringSchedules();
};

#endif
