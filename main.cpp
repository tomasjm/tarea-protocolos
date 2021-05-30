#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "protocol/protocol.h"
#include "helpers/helpers.h"
#include "menu/menu.h"
int main() {
  int option = 0;
  printMenu();
  getOptionAndValidate(&option);
  printf("Selected option %d\n", option);
  doCmdAction(option);
}




