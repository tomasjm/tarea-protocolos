#include <stdio.h>
#include <stdlib.h>
#include "menu.h"

void clearScreen() {
  system("clear");
}

void printMenu(int q, int maxQty) {
  clearScreen();
  printf("Main menu\n");
  printf("Please select an option. Currently we have %d values of temperature\n", q);
  printf("1. Get and replace %d new values of temperature from the sensor \n", maxQty);
  printf("2. Send values of temperature to receptor\n");
  printf("3. Calculate parameters of the values from the receptor\n");
  printf("4. Close receptor\n");
}

void getOptionAndValidate(int *optPtr, int q, int maxQty) {
  scanf("%d", optPtr);
  while(*optPtr < 1 || *optPtr > 4) {
    printMenu(q, maxQty);
    printf("Please select a valid option...\n");
    getOptionAndValidate(optPtr, q, maxQty);
  }
}
