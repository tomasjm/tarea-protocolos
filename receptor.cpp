#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
int maxTemp = 0;
int medianTemp = 0;
int minTemp = 0;

bool parityError = 0;
int errorCount = 0;

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
      if (!transmissionStarted)
        printf("Waiting data ...\n");
      else
        printf("Receiving data ...\n")
          delay(1000);
    }
    generateReceivedFrame(receivedFrame);
    if (receivedFrame.cmd == 4) {
      printf("Closing...\n");
      exit(1);
    } else if (receivedFrame.cmd == 2) {
      if (parityError) {
        errorCount++;
        printf("An error was detected so message is ignored ...\n");
        continue;
      }
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
      dataQty+=1;
      printf("Temp %d\n", temp);
      printf("Se han capturado %d datos\n", dataQty);
      int sum = 0;
      for (int i=0; i<dataQty; i++) {
        if (i == 0)
          minTemp = tempArr[i];
        if (tempArr[i] > maxTemp)
          maxTemp = tempArr[i];
        if (tempArr[i] < minTemp)
          minTemp =tempArr[i];
        sum += tempArr[i];
      }
      medianTemp = sum/dataQty;
      printf("Max Temp %d | Min Temp %d | Median Temp %d \n", maxTemp, minTemp, medianTemp );
    } else if (receivedFrame.cmd == 3) {
      printf("Received messages %d | Errors found %d \n", dataQty, errorCount);
      printf("Max Temp %d | Min Temp %d | Median Temp %d \n", maxTemp, minTemp, medianTemp );
    }
    memset(&receivedFrame, 0, sizeof(receivedFrame));
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
    parity = level;
    nones = (bytes[nbytes]&0x01) + ((bytes[nbytes]&0x02)>>1) + ((bytes[nbytes]&0x04)>>2) + ((bytes[nbytes]&0x08)>>3)
      + ((bytes[nbytes]&0x10)>>4) + ((bytes[nbytes]&0x20)>>5) + ((bytes[nbytes]&0x40)>>6) + ((bytes[nbytes]&0x80)>>7);
    if(parity != (nones%2==0)){
      parityError = true;
    }

    nbytes++;
    transmissionStarted =false;
  }
  nbits++;
}
