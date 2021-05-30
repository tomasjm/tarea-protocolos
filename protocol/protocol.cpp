#include "protocol.h"
#include <stdio.h>
void generateFrameToSend(Frame &f) {
  f.frame[0] = (f.length & 0x7F) | (f.cmd & 0x1) << 7;
  f.frame[1] = (f.cmd & 0xE) >> 1 | (f.sa & 0x1F) << 3;
  for (int i = 0; i<f.length; i++) {
    f.frame[2+i] = f.data[i];
  }
  for (int i = f.length; i<129; i++) {
    f.frame[2+i] = 0;
  }
}

void generateReceivedFrame(Frame &f) {
  f.length = (f.frame[0] & 0x7F);
  f.cmd = (f.frame[0]&0x80) >> 7 | (f.frame[1]&0x7)<<1;
  f.sa = (f.frame[1]&0xF8)>>3;
  for (int i = 0; i<f.length; i++) {
    f.data[i] = f.frame[2+i];
  }
}

void getByteArrayOfInteger(int v, BYTE arr[]) {
  int l = sizeof(v);
  for (int i = 0; i<l; i++) {
    arr[i] = (v >> (8*i)) & 0xFF;
  }
}

void getIntegerOfByteArray(BYTE arr[], int *v_ptr) {
  for (int i = 0; i< sizeof(int); i++) {
    *v_ptr |= (arr[i] << (i*8));
  }
}

