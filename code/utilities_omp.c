#include "utilities.h"
#include "utilities_omp.h"

//////////////////// TO COMPUTE SPLIT AND REORGANIZE DATA ////////////////////////
// This function are used both from the MPI and OMP functions to building the   //
// tree.                                                                        //
//////////////////////////////////////////////////////////////////////////////////

void find_max_min_omp(data* max,data* min, data* set, int dim){
  // Find max and min value of the data-set in input, both on x and y.


  float_t max_x = set[0].x , max_y = set[0].y, min_x = set[0].x , min_y = set[0].y;
  #pragma omp parallel
  {
    #pragma omp for reduction(min:min_x, min_y) reduction(max:max_x,max_y)
    for(size_t i = 0; i < dim; i++){
      if(set[i].x < min_x){
        min_x = set[i].x;
      }
      if(set[i].x > max_x){
        max_x = set[i].x;
      }
      if(set[i].y < min_y){
        min_y = set[i].y;
      }
      if(set[i].y > max_y){
        max_y = set[i].y;
      }
    }
    max->x = max_x;
    max->y = max_y;
    min->x = min_x;
    min->y = min_y;

}

}

int split_and_sort_omp(data* set, data max, data min, int left, int right, int ax){
  // This function find the median of the data-set, then re-organize the data-set
  // in order to have on the left of the median only numbers lower than the median
  // and on the right all the numbers greater or equal. Then the function return the
  // new index of the median (aka split value). The median is computed depending on the
  // ax taken in input.

  int index = left, dim = (right - left + 1), center = dim/2 + left;
  int k=0, h=right-left;
  float_t target;

  if(ax == X){
    target = (max.x-min.x)/2 + min.x;         // I'm using the assumption that the data-set is distributed equally
    index = find_split_index_omp(set, target, left, right, ax);  // Find the index of the value closer to the target (aka the median)
    swap(&set[index], &set[right]);

    data pivot = set[right];
    int i=left -1, j;
    for(j = left; j <= right - 1; j++ ){
      if(set[j].x < pivot.x){
        i++;
        swap(&set[i],&set[j]);
      }
    }
    swap(&set[i+1], &set[right]);
    return i+1;
  }

  if(ax == Y){        // The same routine as before with y
    target = (max.y-min.y)/2 + min.y;
    index = find_split_index_omp(set, target, left, right, ax);

    swap(&set[index], &set[right]);
    data pivot = set[right];
    int i=left -1, j;
    for(j = left; j <= right - 1; j++ ){
      if(set[j].y < pivot.y){
        i++;
        swap(&set[i],&set[j]);
      }
    }
    swap(&set[i+1], &set[right]);
    return i+1;

  }
}

int find_split_index_omp(data* set, float_t target, int left, int right, int ax){
// This function find the index of the value closer to the target on x or y, depending
// on the ax taken in input.

  int index = left;
  int* local_index;
  int dim;
  if(ax == X){
    float_t x = dist(set[index].x, target);

    // Each thread will select the better index for different part of the data-set,
    // then the master select the best among these ones.
    #pragma omp parallel shared(dim, local_index)
    {
      #pragma omp single
      {
        dim=omp_get_num_threads();
        local_index = malloc(sizeof(int)*dim);     // A single thread allocate an array where each thread will
                                                   // store its index.
      }
      #pragma omp for
      for(int i=left; i<=right; i++){           // Each thread find its index on its portion of data
        if(dist(set[i].x, target) < x){
         index = i;
         x = dist(set[index].x, target);
        }
      }
      local_index[omp_get_thread_num()] = index;
    }

    index = local_index[0];
    for(int i=0; i<dim; i++){     // Master thread select the best index
      if(dist(set[local_index[i]].x, target) < dist(set[index].x, target)){
        index = local_index[i];
      }
    }
  }
  if(ax == Y){      // The same routine as above but with y ax instead of x
    float_t x = dist(set[index].x, target);
    #pragma omp parallel shared(dim, local_index)
    {
      #pragma omp single
      {
        dim=omp_get_num_threads();
        local_index = malloc(sizeof(int)*dim);
      }
      #pragma omp for
      for(int i=left; i<=right; i++){
        if(dist(set[i].y, target) < x){
         index = i;
         x = dist(set[index].y, target);
        }
      }
      local_index[omp_get_thread_num()] = index;
    }

    index = local_index[0];
    for(int i=0; i<dim; i++){
      if(dist(set[local_index[i]].y, target) < dist(set[index].y, target)){
        index = local_index[i];
      }
    }
  }
  free(local_index);
  return index;
}



/////////////////// TREE TO ARRAY AND VICEVERSA /////////////////////

node* expand_omp(node* left_tree, node* right_tree, node* tree, int dim,int rcv_dim){
  // This function will store in tree the merge between left_tree and right_tree.
  //                      ------------------------------------------
  //   tree ----->> || [ right_tree ]     |   [ left_tree ] ||
  //                      ------------------------------------------
  //


  #pragma omp parallel
  {
    #pragma omp for
    for(int i=0; i<dim; i++){
      tree[i+1] = left_tree[i];       // Copy left_tree in the first part of tree

      if(tree[i+1].left != -1){
        tree[i+1].left = tree[i+1].left + 1; // Update the index of the left child of the node in tree
      }
      if(tree[i+1].right != -1){
        tree[i+1].right = tree[i+1].right + 1;  // Update the index of the right child of the node in tree
      }
    }
    #pragma omp for
    for(int i=0; i<rcv_dim; i++){
      tree[i+dim+1] = right_tree[i];    // Copy right_tree in the second part of the tree

      if(tree[i+dim+1].left != -1){
        tree[i+dim+1].left = tree[i+dim+1].left + dim + 1;    // Update the index of the left child of the node in tree
      }
      if(tree[i+dim+1].right != -1){
        tree[i+dim+1].right = tree[i+dim+1].right + dim + 1;  // Update the index of the right child of the node in tree
      }
    }
  }
  if(dim > 0){
    free(left_tree);
  }
  if(rcv_dim > 0){
    free(right_tree);
  }
  return tree;
}
