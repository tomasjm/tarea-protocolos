// Tomás Jiménez - Kevin Herrera
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
float tempArr[100];
int timeArr[100];
BYTE tempByteArr[4];
BYTE timeByteArr[4];
extern int cmdReceived = 0;
extern int dataQty = 0;
float maxTemp = 0;
float medianTemp = 0;
float minTemp = 0;

bool parityError = 0;
int errorCount = 0;
bool parity = 0;
int nones = 0;
int errorRangeCount = 0;

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
        printf("Receiving data ...\n");
      delay(1000);
    }
    cmdReceived++;
    if (parityError) {
      printf("An error was detected in parity bit so ignoring message ...\n");
      parityError = false;
      errorCount++;
      memset(&receivedFrame, 0, sizeof(receivedFrame));
      nbytes = 0;
      continue;
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
      int vTime = 0;
      getIntegerOfByteArray(tempByteArr, &temp);
      getIntegerOfByteArray(timeByteArr, &vTime);
      tempArr[dataQty] = ((float)temp-10000)/1000;
      timeArr[dataQty] = vTime;
      if (tempArr[dataQty] < -10.00 || tempArr[dataQty] > 55.00 || timeArr[dataQty] > (int)time(NULL)) {
        printf("An error was detected at value ranges of the data received, so ignoring data ...\n");
        errorRangeCount++;
        continue;
      }
      dataQty+=1;
      printf("Received %d values of temperature with it timestamp\n", dataQty);
      float sum = 0;
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
      printf("Max Temp %.2f | Min Temp %.2f | Median Temp %.2f \n", maxTemp, minTemp, medianTemp );
    } else if (receivedFrame.cmd == 3) {
      printf("Total messages received (CMDs): %d | Total errors found: %d\n",cmdReceived, errorRangeCount + errorCount);
      printf("Errors by parity check: %d | Errors by range check: %d\n", errorCount, errorRangeCount);
      printf("Values of temperature received without errors: %d\n", dataQty);
      printf("---- Percentages of messages received ----\n");
      printf("Without error: %.3f%%   |   Parity error: %.3f%%   |   Range error: %.3f%%   \n", ((float)(cmdReceived-errorRangeCount-errorCount)/cmdReceived)*100, ((float)errorCount/cmdReceived)*100, ((float)errorRangeCount/cmdReceived)*100);
      printf("----- Values related to temperature ----\n");
      printf("Max Temp %.2f | Min Temp %.2f | Median Temp %.2f \n", maxTemp, minTemp, medianTemp );
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
    nones = (receivedFrame.frame[nbytes]&0x01) + ((receivedFrame.frame[nbytes]&0x02)>>1) + ((receivedFrame.frame[nbytes]&0x04)>>2) + ((receivedFrame.frame[nbytes]&0x08)>>3)
      + ((receivedFrame.frame[nbytes]&0x10)>>4) + ((receivedFrame.frame[nbytes]&0x20)>>5) + ((receivedFrame.frame[nbytes]&0x40)>>6) + ((receivedFrame.frame[nbytes]&0x80)>>7);
    if(parity != (nones%2==0)){
      parityError = true;
    }

    nbytes++;
    transmissionStarted =false;
  }
  nbits++;
}
