#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	printf("\ntest main begining, pid=%d\n", getpid());
	long size = 50000000;
  int *data = malloc(sizeof(int) * size);
  if (data == NULL) {
    printf("malloc failed\n");
    exit(-1);
  }
  int res = 0;
  for (long i = 0; i < size; i++) {
    data[i] = i;
    res += data[i];
  }
  for (long i = 0; i < size; i++) {
    data[i] = i;
    res += data[i];
  }
  for (long i = 0; i < size; i++) {
    data[i] = i;
    res += data[i];
  }
  printf("\ntest main ending...\n");
  return res;
}
