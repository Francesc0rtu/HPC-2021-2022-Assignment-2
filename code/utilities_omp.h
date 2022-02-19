#include<stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include<time.h>

void find_max_min_omp(data* max,data* min, data* set, int dim);
int split_and_sort_omp(data* set, data max, data min, int left, int right, int ax);
int find_split_index_omp(data* set, float_t target, int left, int right, int ax);
node* expand_omp(node* left_tree, node* right_tree, node* tree, int dim,int rcv_dim);
