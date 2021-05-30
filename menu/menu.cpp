#include <stdio.h>
#include <stdlib.h>
#include "menu.h"

void clearScreen() {
  system("clear");
}

void printMenu(int q) {
  clearScreen();
  printf("Main menu\n");
  printf("Please select an option. Currently we have %d values of temperature\n", q);
  printf("1. Get 100 values of temperature from the sensor\n");
  printf("2. Send values of temperature to receptor\n");
  printf("3. Calculate parameters of the values from the receptor\n");
  printf("4. Close receptor\n");
}

void getOptionAndValidate(int *opt_ptr, int q) {
  scanf("%d", opt_ptr);
  while(*opt_ptr < 1 || *opt_ptr > 4) {
    printMenu(q);
    printf("Please select a valid option...\n");
    getOptionAndValidate(opt_ptr);
  }
}
