#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

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
BYTE tempArr[4];
BYTE timeArr[4];
Frame frame;
int sensorDataQuantity;


int main() {
  if (start) {
    start = false;
    int tempTestValue = 5555;
    int timeTestValue = 19123456;
    getByteArrayOfInteger(tempTestValue, tempArr);
    getByteArrayOfInteger(timeTestValue, timeArr);
    frame.cmd = 7;
    frame.length = 8;
    for (int i = 0; i<4; i++) {
      frame.data[i] = tempArr[i];
      frame.data[i+4] = timeArr[i]; 
    }
    generateFrameToSend(frame);

  }
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
  printMenu(sensorDataQuantity);
  getOptionAndValidate(&option, sensorDataQuantity);
  if (option == 1) {    
    int q = 100;
    int valueArr[q];
    int timeArr[q];
    readSensorData(q, valueArr, timeArr);
    sensorDataQuantity +=q;
  } else if (option ==2 ) {
    frame.cmd = 2;
    frame.length = 8;
    printf("no implementado \n");
    generateFrameToSend(frame);
    startTransmission();
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
    printf("nbits %d | nbytes %d | qty %d\n", nbits, nbytes, sensorDataQuantity);
    delay(100);
  }
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
    // calcular bit paridad
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
