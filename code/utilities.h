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
data value;
int AxSplit;
int depth;
int left;
int right;
} node;

typedef struct L {
data value;
int AxSplit;
int depth;
struct L *left;
struct L *right;
} knode;


//////////////////////// HEADER /////////////////////////
void print(data* set, int dim);
void find_max_min(data* max,data* min, data* set, int dim);
int split_and_sort(data* set, data min, data max, int left, int right, int ax);
void print_ktree(knode* root);
void print_array_node(node* array, int dim);
node* tree_to_array(knode* root, int dim);
int map_to_array(node* array, knode* root, int dim, int i);
void map_to_tree(knode* array, knode* root, int dim, int i);
void print_ktree_ascii(knode* root, int space);
node* expand(node* array_tree, node* rcv_array, node* merge_array, int dim,int rcv_dim);
