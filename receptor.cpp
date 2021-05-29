#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#include "protocol/protocol.h"
#include "helpers/helpers.h"

#define CLOCK_PIN 23
#define TX_PIN 22
#define RX_PIN 21

#define BYTE unsigned char

void processBit(bool level);
void cb(void);


volatile int nbits = 0;
volatile int nbytes = 0;
bool transmissionStarted =false;
bool start = true; 
Frame receivedFrame;


int main() {
  if (start) {
    start =false;
  }
  if (wiringPiSetup() == -1)
    exit(1);
  piHiPri(99);
  pinMode(RX_PIN,INPUT);
  pinMode(TX_PIN,OUTPUT);

  if (wiringPiISR(CLOCK_PIN, INT_EDGE_RISING, &cb) < 0) {
    printf("Unable to start interrupt function\n");
  }

  while (nbytes < 10)
    delay(300);

  generateReceivedFrame(receivedFrame);
  int temp = 0;
  int time = 0;
  BYTE tempArr[4];
  BYTE timeArr[4];
  for (int i = 0; i<4; i++) {
    tempArr[i] = receivedFrame.data[i];
    timeArr[i] = receivedFrame.data[i+4];
  }
  getIntegerOfByteArray(tempArr, &temp);
  getIntegerOfByteArray(timeArr, &time);
  printf("Length: %d | CMD: %d | SA: %d\n", receivedFrame.length, receivedFrame.cmd, receivedFrame.sa);
  printf("Temp: %d | Time: %d\n", temp, time);
}

void cb(void) {
  bool level = digitalRead(RX_PIN);
  if (transmissionStarted) {
    processBit(level);
  } else if (level == 0 && !transmissionStarted) {
    transmissionStarted = true;
    nbits = 1;
  }
}

void processBit(bool level) {
  if (nbits < 9) {
    receivedFrame.frame[nbytes] |= level << (nbits-1);
  } else if (nbits == 9) {
    // check paridad
    nbytes++;
    transmissionStarted =false;
  }
  nbits++;
}
