#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers/helpers.h"
#include "protocol/protocol.h"
#include "menu/menu.h"

#define CLOCK_PIN 0
#define TX_PIN 2
#define RX_PIN 3

#define BYTE unsigned char

void cb(void);
void startTransmission();
void getSensorData();


bool transmissionStarted = false;
volatile int nbits = 0;
volatile int cmd = 0;
volatile int sa = 0;
volatile int nbytes = 0;
bool start = false;
BYTE tempByteArr[4];
BYTE timeByteArr[4];
int tempArr[100];
int timeArr[100];
Frame frame;
int sensorDataQuantity[1] = {0};
extern int q = 5;
int nones = 0;
int main() {
  if (wiringPiSetup() == -1)
    exit(1);

  if (wiringPiISR(CLOCK_PIN, INT_EDGE_RISING, &cb) < 0) {
    printf("Unable to start interrupt function\n");
  }

  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
  delay(5000);
  int option = 0;
  while (true) {
    printMenu(sensorDataQuantity[0], q);
    getOptionAndValidate(&option, sensorDataQuantity[0], q);
    if (option == 1) {    
      readSensorData(q,tempArr, timeArr);
      sensorDataQuantity[0] +=q;
    } else if (option ==2 ) {
      frame.cmd = 2;
      frame.length = 8;
      int i = 0;
      while (i<q) {
        getByteArrayOfInteger(tempArr[i], tempByteArr);
        getByteArrayOfInteger(timeArr[i], timeByteArr);
        for (int i = 0; i<4; i++) {
          frame.data[i]=tempByteArr[i];
          frame.data[i+4]=timeByteArr[i];
        }
        generateFrameToSend(frame);
        startTransmission();
        while(transmissionStarted) {
          printf("Enviando datos. Cant enviada: %d\n", i);
          delay(1000);
        }
        i++;
        delay(1000);
      }
    } else if (option == 3) {
      frame.cmd = 3;
      frame.sa = 2;
      frame.length = 0;
      generateFrameToSend(frame);
      printf("Sending command to calculate params ... \n");
      startTransmission();
    } else if (option ==4) {
      frame.cmd = 4;
      frame.sa = 4;
      frame.length = 0;
      generateFrameToSend(frame);
      printf("Sending command to close...\n");
      startTransmission();
    }
    while(transmissionStarted) {
      printf("Sending data ...\n");
      delay(100);
    }
    memset(&frame, 0, sizeof(frame));
  }
  return 0;
}

void cb(void) {
  if (!transmissionStarted) {
    return digitalWrite(TX_PIN,1);
  }
  if (nbits == 0) 
    digitalWrite(TX_PIN, 0);
  if (nbits < 9 && nbits != 0)
    digitalWrite(TX_PIN, (frame.frame[nbytes] >> (nbits-1)&0x1));
  if (nbits == 9) {
    nones = (frame.frame[nbytes]&0x01) + ((frame.frame[nbytes]&0x02)>>1) + ((frame.frame[nbytes]&0x04)>>2) + 
      ((frame.frame[nbytes]&0x08)>>3) + ((frame.frame[nbytes]&0x10)>>4) + ((frame.frame[nbytes]&0x20)>>5) + 
      ((frame.frame[nbytes]&0x40)>>6) + ((frame.frame[nbytes]&0x80)>>7);
    digitalWrite(TX_PIN, nones%2==0); 
  }
  if (nbits > 9)
    digitalWrite(TX_PIN, 1);
  nbits++;

  if (nbits == 11) {
    nbits = 0;
    nbytes++;
    if (nbytes == 10) {
      transmissionStarted = false;
      nbytes = 0;
    }
  }
}

void startTransmission() {
  transmissionStarted = true;
}
