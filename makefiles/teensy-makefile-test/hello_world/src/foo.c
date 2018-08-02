#include "foo.h"

int* foo() {
  int *bar = (int*)calloc(5,sizeof(int));
  int i=0;
  for(i=0;i<5;++i)
    bar[i]=i;
  return bar;
}