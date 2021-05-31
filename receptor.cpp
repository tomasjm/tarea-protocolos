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
int tempArr[100];
int timeArr[100];
BYTE tempByteArr[4];
BYTE timeByteArr[4];
extern int dataQty = 0;
int main() {
  if (wiringPiSetup() == -1)
    exit(1);
  piHiPri(99);
  pinMode(RX_PIN,INPUT);
  pinMode(TX_PIN,OUTPUT);

  if (wiringPiISR(CLOCK_PIN, INT_EDGE_RISING, &cb) < 0) {
    printf("Unable to start interrupt function\n");
  }
  while (true) {
    while (nbytes < 10) {
      printf("nbytes %d\n", nbytes);
      delay(1000);
    }
    generateReceivedFrame(receivedFrame);
    if (receivedFrame.cmd == 4) {
      printf("Closing...\n");
      exit(1);
    } else if (receivedFrame.cmd == 2) {
      for (int j = 0; j<4;j++) {
        tempByteArr[j] = receivedFrame.data[j];
        timeByteArr[j] = receivedFrame.data[j+4];
      }
      int temp = 0;
      int time = 0;
      getIntegerOfByteArray(tempByteArr, &temp);
      getIntegerOfByteArray(timeByteArr, &time);
      tempArr[dataQty] = temp;
      timeArr[dataQty] = time;
      printf("Se han capturado %d datos\n", dataQty);
    }
    nbytes = 0;
  }
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
