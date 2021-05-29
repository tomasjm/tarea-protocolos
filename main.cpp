#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "protocol/protocol.h"
#include "helpers/helpers.h"

int main() {
  Frame frame, c_frame;
  frame.cmd = 50;
  frame.sa = 7;
  float tempf = 55.55f;
  int temp = (int)(tempf*100);
  int currentTime = (int)time(NULL);
  frame.length = sizeof(temp) + sizeof(currentTime);
  BYTE tempArr[4];
  BYTE timeArr[4];
  getByteArrayOfInteger(temp, tempArr);
  getByteArrayOfInteger(currentTime, timeArr);
  for (int i = 0; i<8; i++){
    if (i < 4) frame.data[i] = tempArr[i];
    else frame.data[i]=timeArr[i-4];  
  }
  generateFrameToSend(frame);
  // se genera la transmisión
  for (int i = 0; i<129; i++) {
    c_frame.frame[i] = frame.frame[i];
  } //simulacion de transmision
  generateReceivedFrame(c_frame);
  printf("cmd: %d | sa: %d | length: %d \n", c_frame.cmd, c_frame.sa, c_frame.length);
  int dtemp = 0;
  int dtime = 0;
  BYTE dtemparr[4];
  BYTE dtimearr[4];
  for (int i = 0; i<4; i++) {
    dtemparr[i] = c_frame.data[i];
    dtimearr[i] = c_frame.data[i+4];
  }
  getIntegerOfByteArray(dtemparr, &dtemp);
  getIntegerOfByteArray(dtimearr, &dtime);
  printf("valor: %.2f\n", (float)dtemp/100);
  printf("currentime: %d\n", dtime);
  printf("largo %d\n", frame.length);
}




