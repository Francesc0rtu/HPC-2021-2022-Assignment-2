#include "utilities.h"
#include "omp_tree.h"
#include <unistd.h>
#include <sched.h>

#define _GNU_SOURCE
//////////////////////////// MULTI-THREADING BUILDING TREE ///////////////////////////////////////////////////////////
// This function take in input the dataset and return the tree. The tree is saved as an array of                    //
// the struct node:                                                                                                 //
//                            -------------------------------                                                       //
//                            |           (x,y)             |  <- (x,y) is the point                                //
//      struct node ----->>   -------------------------------                                                       //
//                            | left | right | axis | depth |   <- left and right are the index                     //
//                            -------------------------------     of the left child and right child                 //
//                                                                (-1 if the child doesn't exist)                   //
//                                                                                                                  //
// The array is saved in a in-order visit of the tree: in this way merging a node with its left and                 //
// right sub-tree is simply done like this:                                                                         //
// --------       ------------------------      --------------------     ----------------------------------------   //
// | node |   +   |  left sub tree        |  +  | right sub tree   |  == |node | left sub tree | right sub tree |   //
// --------       ------------------------      --------------------     ----------------------------------------   //
//                                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

node* build_omp_tree(data* set, int left,int right,int ax, int depth){
  // This function recive in input the data-set, the left and right index of the data-set that
  // it will be considered, the ax of the splitting and the depth of the tree.

  int rank; MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  // printf("thread=%d, cpu=%d\n",omp_get_thread_num(), sched_getcpu());  

  if(left==right){  // If left == right it returns a leaf
    node* aux;
    aux = malloc(sizeof(node));
    aux->value = set[left];
    aux->AxSplit = ax;
    aux->depth = depth;
    aux->left = -1;
    aux->right = -1;
    return aux;
  }
  if(left < right){
    node *merged,*left_array,*right_array;
    data max,min;
    int index_split, dim = right - left + 1,left_dim,right_dim;

    find_max_min(&max,&min, set+left, dim);                   // Find max and min in the portion of data considered
    index_split = split_and_sort(set, max,min,left,right,ax); // Find the index of the splitting value

    merged = malloc(sizeof(node)*dim);              // allocate the dimension of the new array
    merged[0].value = set[index_split];             // In the index 0 there is the root
    merged[0].AxSplit = ax;
    merged[0].depth = depth;

    if(left <= index_split -1){                   // If exist a left child(or a sub-tree)
      #pragma omp task firstprivate(set,left,right,index_split,ax,depth) shared(left_array)
      {left_array = build_omp_tree(set,left, index_split -1, 1-ax, depth+1);}
      left_dim = index_split - left;
      merged[0].left = 1;                 // The left child of the father is always the array[1]
    }else{                      // In this case doesn't exist a left child
      left_dim = 0;
      merged[0].left = -1;
    }

    if(index_split + 1 <= right){              // If exist a right child(or a sub-tree)
      #pragma omp task firstprivate(set, index_split,right,ax,depth) shared(right_array)
      {right_array = build_omp_tree(set,index_split + 1, right, 1-ax, depth+1);}
      right_dim = right - index_split ;
      if(left_dim == 0){                    // If the left child doesn't exist the right child is in the position array[1]
        merged[0].right = 1;
      }else{
        merged[0].right = left_dim + 1;
      }
    }else{                              // In this case the right child doesn't exist
      right_dim=0;
      merged[0].right = -1;
    }
    // To merge I have to wait that the task are done
    #pragma omp taskwait
    merged = expand(left_array, right_array, merged,left_dim, right_dim);
    // To merge I have to wait that the task are done

    return merged;
  }
}
