#include "utilities.h"
#define COUNT 5

/////////////////////////////// WHAT ARE THESE FUNCTIONS ?? ///////////////////////////
// In this file there are all the utilities functions used by the other part of the  //
// program. In the file "utilities.h" there are all the declarantion of struct and   //
// macro used by the others part of the program.                                     //
//                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////



/////////////////// PRINT FUNCTIONS ////////////////////

void print(data* set, int dim){
  // Print the data-set on standard output

  for (size_t i = 0; i < dim; i++) {
    printf("(%f,%f), ", set[i].x, set[i].y);
  }printf("\n");
}


void print_tree_ascii(node *root, int space, int i){
  // Print a graphical rappresention of the tree in ASCII on standard output.
  // The result is human readable only for a small size of the input ~100-400.

  // Increase distance between levels
  space += COUNT;

  // Process right child first
  if(root[i].right != -1){
    print_tree_ascii(root, space, root[i].right);
  }

  // Print current node after space
  // count
  printf("\n");
  for (int j = COUNT; j < space; j++)
      printf(" ");
  printf("[(%f,%f),%d,%d]\n", (root[i].value).x, (root[i].value).y, root[i].AxSplit, root[i].depth);

  // Process left child
  if(root[i].left != -1){
    print_tree_ascii(root, space, root[i].left);
  }
}

void print_tree(node* array, int dim){
  // Print on std output a list of nodes with the following format:
  // [index in the array]_{(x,y),lh=<index of left child>, rh=<index of right child>, ax=<ax of splitting>}

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("------------%d-----------\n",rank);
  for(int i=0; i<dim; i++){
    printf("[%d]_{(%f,%f),lh=%d,rh=%d,ax=%d} ", i,(array[i].value).x, (array[i].value).y, array[i].left,array[i].right,array[i].AxSplit );
  }printf("\n");
}

//////////////////// TO COMPUTE SPLIT AND REORGANIZE DATA ////////////////////////
// This function are used both from the MPI and OMP functions to building the   //
// tree.                                                                        //
//////////////////////////////////////////////////////////////////////////////////

void find_max_min(data* max,data* min, data* set, int dim){
  // Find max and min value of the data-set in input, both on x and y.


  float_t max_x = set[0].x , max_y = set[0].y, min_x = set[0].x , min_y = set[0].y;
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

int split_and_sort(data* set, data max, data min, int left, int right, int ax){
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
    index = find_split_index(set, target, left, right, ax);  // Find the index of the value closer to the target (aka the median)
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
    index = find_split_index(set, target, left, right, ax);

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

int find_split_index(data* set, float_t target, int left, int right, int ax){
// This function find the index of the value closer to the target on x or y, depending
// on the ax taken in input.

  int index = left;
  int* local_index;
  int dim;
  if(ax == X){
    float_t x = dist(set[index].x, target);
      for(int i=left; i<=right; i++){           // Each thread find its index on its portion of data
        if(dist(set[i].x, target) < x){
         index = i;
         x = dist(set[i].x, target);
        }
      }
    }
  if(ax == Y){      // The same routine as above but with y ax instead of x
    float_t x = dist(set[index].x, target);
      for(int i=left; i<=right; i++){
        if(dist(set[i].y, target) < x){
          index = i;
          x = dist(set[i].y, target);
        }
      }
    }

  return index;
}

float_t dist(float_t x, float_t y){
  // Compute the distance between two numbers
  if(y>=x) return (y-x);
  else return x-y;
}

void swap(data* x, data* y){
  // Basic swap
  data tmp;
  tmp = *x;
  *x = *y;
  *y = tmp;
}
/////////////////// TREE TO ARRAY AND VICEVERSA /////////////////////

node* expand(node* left_tree, node* right_tree, node* tree, int dim,int rcv_dim){
  // This function will store in tree the merge between left_tree and right_tree.
  //                      ------------------------------------------
  //   tree ----->> || [ left_tree ]     |   [ right_tree ] ||
  //                      ------------------------------------------
  //



    for(int i=0; i<dim; i++){
      tree[i+1] = left_tree[i];       // Copy left_tree in the first part of tree
      if(tree[i+1].left != -1){
        tree[i+1].left = tree[i+1].left + 1; // Update the index of the left child of the node in tree
      }
      if(tree[i+1].right != -1){
        tree[i+1].right = tree[i+1].right + 1;  // Update the index of the right child of the node in tree
      }
    }
    for(int i=0; i<rcv_dim; i++){
      tree[i+dim+1] = right_tree[i];    // Copy right_tree in the second part of the tree
      if(tree[i+dim+1].left != -1){
        tree[i+dim+1].left = tree[i+dim+1].left + dim + 1;    // Update the index of the left child of the node in tree
      }
      if(tree[i+dim+1].right != -1){
        tree[i+dim+1].right = tree[i+dim+1].right + dim + 1;  // Update the index of the right child of the node in tree
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
