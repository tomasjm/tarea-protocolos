#include <time.h>
#include "helpers.h"

void cDelay(int milliSeconds) {
  clock_t startTime = clock()/CLOCKS_PER_SEC;
  while (clock()/CLOCKS_PER_SEC < startTime + milliSeconds/1000);
}

void readSensorData(int q, int valuesArr[], int timesArr[]) {
  for (int i = 0; i<q; i++) {
    cDelay(100);
    valuesArr[i] = 1000;
    timesArr[i] = (int)time(NULL);
  }
}
