#include<stdio.h>
#include <stdlib.h>


//MACRO
#ifndef DOUBLE_PRECISION
#define float_t float
#else
#define float_t double
#endif

int main(int argc, char const *argv[]) {
  int a;
  float b;
  double c;
  scanf("%ld", &a);
  printf("%ld, float:%ld, double:%ld\n", sizeof(float_t), sizeof(float), sizeof(double));
  return 0;
}
