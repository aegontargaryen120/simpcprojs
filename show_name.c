#include <stdio.h>

int main(void) {
  char *name = NULL;
  
  printf("Enter your name: ");
  scanf("%s", &name);
  printf("\n");

  printf("Hello, %s!\n", name);
  return 0;
}
