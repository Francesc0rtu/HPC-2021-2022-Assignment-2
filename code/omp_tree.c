#include "utilities.h"
#include <unistd.h>
knode* build_omp_tree(data* set,int left,int right,int ax, int depth){
  if(left==right){
    knode* tmp;
    tmp = malloc(sizeof(knode));
    tmp->value = set[left];
    tmp -> AxSplit = ax;
    tmp -> depth = depth;
    tmp -> left = NULL;
    tmp -> right = NULL;
    return tmp;
  }
  if(left > right){
    return NULL;
  }
  if(left < right){
  data max, min;
  int index_split;
  knode *tmp;
  tmp = malloc(sizeof(knode));
  find_max_min(&max, &min, set+left , right-left +1);
  index_split = split_and_sort(set, max ,min, left, right, ax);
  // printf("// node=(%d,%d), axis=%d, max=(%d,%d), min=(%d,%d) \n",
        // set[index_split].x,set[index_split].y,ax,max.x,max.y,min.x,min.y);
  tmp->value = set[index_split];
  tmp->AxSplit = ax;
  tmp->depth = depth;

#pragma omp task firstprivate(set,left,right,index_split,ax, depth)
  tmp->left = build_omp_tree(set, left, index_split-1, 1-ax, depth+1);
#pragma omp task firstprivate(set,left,right,index_split,ax, depth)
  tmp->right = build_omp_tree(set, index_split+1, right, 1-ax, depth+1);

  return tmp;
  }
}

void build_omp_array_tree(data* set,node* tree, int left,int right,int ax, int depth, int* i){
  if(left==right){
    *i++;
    tree[*i].value = set[left];
    tree[*i].AxSplit = ax;
    tree[*i].depth = depth;
    tree[*i].left = -1;
    tree[*i].right = -1;
  }
  if(left > right){
  }
  if(left < right){
  data max, min;
  int index_split;
  find_max_min(&max, &min, set+left , right-left +1);
  index_split = split_and_sort(set, max ,min, left, right, ax);
  *i++;
  tree[*i].value = set[index_split];
  tree[*i].AxSplit = ax;
  tree[*i].depth = depth;
  printf("i+++ = %d \n",*i);
// #pragma omp task firstprivate(set,left,right,index_split,ax, depth)
  build_omp_array_tree(set, tree, left, index_split-1, 1-ax, depth+1,i);
// #pragma omp task firstprivate(set,left,right,index_split,ax, depth)
   build_omp_array_tree(set, tree, index_split+1, right, 1-ax, depth+1,i);

  }
}
