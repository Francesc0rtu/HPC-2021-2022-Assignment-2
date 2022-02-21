/////////////////////////////// ///////////////////////////////////////////////////////
// In this file there are all the include,macro and declaration used by all          //
// part of the program.                                                              //
///////////////////////////////////////////////////////////////////////////////////////


#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#include<stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <time.h>
#include <sched.h>
#include <limits.h>
#include <stdint.h>

#define CPU_TIME (clock_gettime( CLOCK_REALTIME, &ts ), (double)ts.tv_sec+(double)ts.tv_nsec*1e-9)

#if !defined(DOUBLE_PRECISION)
#define float_t float
#else
#define float_t double
#endif


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

#define COUNT 5
///////////////////////// STRUCT /////////////////////////////////

typedef struct {
float_t x;
float_t y;
} data;

typedef struct {
data value;
int AxSplit;
int depth;
int left;
int right;
} node;



//////////////////////// DECLARATION /////////////////////////
void print(data* set, int  dim);
void print_tree(node* array, int dim);
void print_tree_ascii(node* root, int space, int  i);

void find_max_min(data* max,data* min, data* set, int  dim);
int split_and_sort(data* set, data min, data max, int  left, int  right, int ax);
int find_split_index(data* set, float_t target, int  left, int  right, int ax);

void find_max_min_omp(data* max,data* min, data* set, int  dim);
int split_and_sort_omp(data* set, data max, data min, int  left, int  right, int ax);
int find_split_index_omp(data* set, float_t target, int left, int right, int ax);

float_t dist(float_t x, float_t y);
void swap(data* x, data* y);

node* expand_serial(node* left_tree, node* right_tree, node* tree, int  dim,int  rcv_dim); 
node* expand(node* array_tree, node* rcv_array, node* merge_array, int  dim,int  rcv_dim); //not used
