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

node* build_omp_array_tree(data* set, int left,int right,int ax, int depth){
  if(left==right){
    node* tmp;
    tmp = malloc(sizeof(node));
    tmp->value = set[left];
    tmp->AxSplit = ax;
    tmp->depth = depth;
    tmp->left = -1;
    tmp->right = -1;
    return tmp;
  }
  if(left < right){
    node* merged,*left_array,*right_array;
    data max,min;
    int index_split, dim = right - left + 1,left_dim,right_dim;
    find_max_min(&max,&min, set+left, dim);
    index_split = split_and_sort(set, max,min,left,right,ax);
    merged = malloc(sizeof(node)*dim);
    merged[0].value = set[left];
    merged[0].AxSplit = ax;
    merged[0].depth = depth;
    if(left <= index_split -1){
      #pragma omp task firstprivate(set,left,right,index_split,ax,depth) shared(left_array)
      left_array = build_omp_array_tree(set,left, index_split -1, ax,depth);
      left_dim=index_split - left;
      merged[0].left = 1;
    }else{
      left_dim=0;
      merged[0].left = -1;
    }
    if(index_split + 1 <= right){
      #pragma omp task firstprivate(set, index_split,right,ax,depth) shared(right_array)
      right_array = build_omp_array_tree(set,index_split + 1, right, ax,depth);
      right_dim = right - index_split ;
      if(left_dim == 0){
        merged[0].right = 1;
      }else{
        merged[0].right = left_dim + 1;
      }
    }else{
      right_dim=0;
      merged[0].right = -1;
    }
    #pragma omp taskwait
    merged = expand(left_array, right_array, merged,left_dim, right_dim);
    return merged;
  }
}
