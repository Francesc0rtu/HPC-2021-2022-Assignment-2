#include<stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include<time.h>

// #if !defined(DOUBLE_PRECISION)
// #define float_t float
// #else
// #define float_t double
// #endif

#define float_t int

#if !defined(DOUBLE_PRECISION)
#define MPI_FLOAT_T MPI_FLOAT
#else
#define MPI_FLOAT_T MPI_DOUBLE
#endif

#define MASTER 0
#define TRUE 1
#define FALSE 0

#define X 0
#define Y 1
///////////////////////// STRUCT /////////////////////////////////

typedef struct {
float_t x;
float_t y;
} data;

typedef struct {
float_t x;
float_t y;
int AxSplit;
int left;
int right;
} node;

//////////////////////// HEADER /////////////////////////
void print(data* set, int dim);
void find_max_min(data* max,data* min, data* set, int dim);
int split_and_sort(data* set, data min, data max, int left, int right, int ax);
float_t dist(float_t x, float_t y);
void swap(data* x, data* y);
