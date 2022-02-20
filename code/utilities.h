#include<stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include<time.h>

#if !defined(DOUBLE_PRECISION)
#define float_t float
#else
#define float_t double
#endif

// #define float_t int

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



//////////////////////// HEADER /////////////////////////
void print(data* set, int dim);
void print_tree(node* array, int dim);
void print_tree_ascii(node* root, int space, int i);
void find_max_min(data* max,data* min, data* set, int dim);
int split_and_sort(data* set, data min, data max, int left, int right, int ax);
int find_split_index(data* set, float_t target, int left, int right, int ax);
float_t dist(float_t x, float_t y);
void swap(data* x, data* y);
node* expand(node* array_tree, node* rcv_array, node* merge_array, int dim,int rcv_dim);
