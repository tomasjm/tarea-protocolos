#include <time.h>
#include "helpers.h"

void cDelay(int milliSeconds) {
  clock_t startTime = clock()/CLOCKS_PER_SEC;
  while (clock()/CLOCKS_PER_SEC < startTime + milliSeconds/1000);
}

void readSensorData(int q, int arr[]) {
  for (int i = 0; i<q; i++) {
    cDelay(1000);
    arr[i] = 1000;
  }
}
