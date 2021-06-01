#include <time.h>
#include <stdio.h>
#include "helpers.h"

void cDelay(int milliSeconds) {
  clock_t startTime = clock()/CLOCKS_PER_SEC;
  while (clock()/CLOCKS_PER_SEC < startTime + milliSeconds/1000);
}

void readSensorData(int q, int valuesArr[], int timesArr[]) {
  FILE *fp;
  int rawTemp;
  for (int i = 0; i<q; i++) {
  fp = popen("cat /sys/bus/w1/devices/28-0113126a6baa/w1_slave | grep -i -o \"t=[0-9]*\" | grep -o \"[0-9]*\"", "r");
  fscanf(fp, "%d", &rawTemp);
  valuesArr[i] = rawTemp+10000;
  timesArr[i] = time(NULL);
  printf("Se han tomado %d datos de %d\n", i+1, q);
  printf("Temp %d\n", rawTemp);
  pclose(fp);
  }
}
