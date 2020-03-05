#ifndef WateringSchedules_h
#define WateringSchedules_h

#include <ArduinoJson.h>

class WateringSchedules {
    const String endpoint = "https://4ab8a3ec.ngrok.io/wateringManagement/schedules/";
    const String idWatering = "5e5596e321bba050aabba187";

    int **schedulesWatering;
    int sizeOfSchedulesWatering;

  public:
    void printSchedules();
    int **getSchedules();
    int getSizeOfSchedulesWatering();
    void getWateringSchedules();
};

#endif
