#include "utilities.h"

knode* build_omp_tree(data* set,int left,int right,int ax){
  if(left==right){
    knode* tmp;
    tmp = malloc(sizeof(knode));
    tmp->value = set[left];
    tmp -> AxSplit = ax;
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
  printf("// node=(%d,%d), axis=%d, max=(%d,%d), min=(%d,%d) \n",
        set[index_split].x,set[index_split].y,ax,max.x,max.y,min.x,min.y);
  tmp->value = set[index_split];
  tmp->AxSplit = ax;
#pragma omp task firstprivate(set,left,right,index_split,ax)
  tmp->left = build_omp_tree(set, left, index_split-1, 1-ax);
#pragma omp task firstprivate(set,left,right,index_split,ax)
  tmp->right = build_omp_tree(set, index_split+1, right, 1-ax);

  return tmp;
  }
}
