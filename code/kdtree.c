#include<stdio.h>
#include <stdlib.h>
//#include <mpi.h>
#include <omp.h>

//MACRO
#if !defined(DOUBLE_PRECISION)
#define float_t float
#else
#define float_t double
#endif


typedef struct {
float_t x;
float_t y;
} data_type;

int main(int argc, char *argv[]){

}
