#include "tree.h"
#include "omp_tree.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// In this file there are the functions to construct a 2d-tree with openMP.                                  //
// The structs used only in this file, such the node of a tree and the elements of stack are cointened in   //
// the respective header "omp_tree.h".                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


node* build_omp_tree(data* set,int dim, int ax, int depth){
  // This function take in input the dataset and the ax of split, then call the build_tree inside a parallel region
  // that return the tree (build with pointer and struct). Than convert the tree in an array and return this array


  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);                     // Print time
  tree_node* root;
  node *vtree;
  struct timespec ts;
  double tstart, tend, omp_time;

  tstart = CPU_TIME;      // start time
  #pragma omp parallel      
     {
      #pragma omp single
      root = build_tree(set, 0, dim -1 , ax, depth);      // Return tree
     }

  tend = CPU_TIME;                  // end time
  omp_time = tend - tstart;    
  tstart = CPU_TIME;                      // start time to compute the conversion time 
  vtree = malloc(sizeof(node)*dim);       // Allocate array to store the tree
  tree_to_array(root, vtree, dim);         // Convert tree to array
  tend = CPU_TIME;

  if(rank == 0){
    FILE *fptr;
    fptr = fopen("../output/time", "a");
    fprintf(fptr,"\t%f,\t%f,", omp_time, tend-tstart);
    fclose(fptr);
  }

  return vtree;                                     // Return array
}

tree_node* build_tree(data* set, int left,int right,int ax, int depth){
  // Build in parallel the tree

  if(left > right){
    return NULL;
  }

  if(left==right){        // return a leaf
    tree_node *root;
    root = malloc(sizeof(tree_node));
    root -> value = set[left];
    root -> AxSplit = ax;
    root -> depth = depth;
    root -> dim_sub_left = 0;
    root -> dim_sub_right = 0;
    root -> left = NULL;   
    root -> right = NULL;
    return root;
  }

  if(left<right){
    data max,min;
    tree_node *root;
    int index_split, dim = right - left , left_dim, right_dim;
    find_max_min(&max,&min, set + left , dim);                   // Find max and min in the portion of data considered
    index_split = split_and_sort(set, max,min,left,right,ax);  // Find the index of the splitting value
    root = malloc(sizeof(tree_node));
    root -> value = set[index_split];
    root -> AxSplit = ax;
    root -> depth = depth;

    left_dim = (index_split - left);
    right_dim = (right- index_split );

    if(left_dim > 0){                         // Compute dimension of subtrees--needed to convert tree to array
      root -> dim_sub_left = left_dim;
    }else {root -> dim_sub_left = 0;}
    if(right_dim > 0){
      root -> dim_sub_right = right_dim;
    }else {root -> dim_sub_right = 0;}

    #pragma omp task firstprivate(left,index_split)           // recursive calls multi-threading
      root -> left = build_tree(set,left,index_split -1, 1-ax, depth+1);
    #pragma omp task firstprivate(right,index_split)
      root -> right = build_tree(set, index_split+1, right, 1-ax, depth+1);
    return root;
  }
}

void tree_to_array(tree_node* root, node* vtree, int dim){
  // Convert a tree to array with a in-order visit of the tree.
  // The tree is stored in an array of struct of this type:                              
  //                                                                                                                       
  //                      ------------------------------                                                                    
  //                      |          (x,y)             |                                                                    
  //  node--------------> ------------------------------                                                                    
  //                      | left |  right | ax | depth |                                                                    
  //                      ------------------------------                                                                    
  //                                                                                                                        
  // where left and right are the index in the array of the left and right child (-1 if there is no left/right child).      
  // ax is the axis of splitting and depth is the level of the node in the tree.                                            


  stack_node* stack; stack=NULL;  // Initialize a stack
  int i=0;
  push(&stack, root);

  while(stack != NULL){
    tree_node * tmp;
    tmp = pop(&stack);
    vtree[i].value = tmp -> value;
    vtree[i].AxSplit = tmp -> AxSplit;
    vtree[i].depth = tmp -> depth;
    vtree[i].right = i +1 + tmp->dim_sub_left;
    
    if(tmp->right != NULL){
      push(&stack, tmp->right);
    }else{
      vtree[i].right = -1;
    }
    if(tmp->left != NULL){
      push(&stack, tmp->left);
      vtree[i].left = i+1;
    }else{
      vtree[i].left = -1;
    }
    free(tmp);
    i++;
  }
}

void push(stack_node** stack, tree_node *point){
  // Simple push of a stack
  stack_node *new;
  new = malloc(sizeof(stack_node));
  new -> elem = point;
  new -> next = *stack;
  *stack = new;
}

tree_node *pop(stack_node** stack){
  // Simple pop of a stack
  stack_node *aux;
  tree_node* value;
  aux = *stack;
  value = aux -> elem;
  *stack = (*stack) -> next;
  free(aux);
  return value;
}
