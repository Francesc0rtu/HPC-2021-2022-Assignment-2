#include "utilities.h"
#include "omp_tree.h"
#include <unistd.h>
#include <sched.h>


node* build_omp_tree(data* set,int dim, int ax, int depth){
  tree_node* root;
  node *vtree;
  int rank;
  double omp_time, convert_time;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  omp_time = omp_get_wtime();
  root=build_tree(set, 0, dim -1 , ax, depth);
  omp_time = omp_get_wtime() - omp_time;

  vtree = malloc(sizeof(node)*dim);
  convert_time = MPI_Wtime();
  tree_to_array(root, vtree, dim);
  convert_time = MPI_Wtime() - convert_time ;

  if(rank == 0){
    FILE *fptr;
    fptr = fopen("../output/time", "a");
    fprintf(fptr,"\t%f,\t%f,", omp_time, convert_time);
    fclose(fptr);
  }
  return vtree;
}

tree_node* build_tree(data* set, int left,int right,int ax, int depth){
  printf("thread:%d, cpu=%d\n", omp_get_thread_num(), sched_getcpu());
  if(left > right){
    return NULL;
  }
  if(left==right){
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
    int index_split, dim = right - left + 1, left_dim, right_dim;
    find_max_min(&max,&min, set+left, dim);                   // Find max and min in the portion of data considered
    index_split = split_and_sort(set, max,min,left,right,ax); // Find the index of the splitting value
    root = malloc(sizeof(tree_node));
    root -> value = set[index_split];
    root -> AxSplit = ax;
    root -> depth = depth;

    left_dim = (index_split - left);
    right_dim = (right- index_split );
    if(left_dim > 0){
      root -> dim_sub_left = left_dim;
    }else {root -> dim_sub_left = 0;}
    if(right_dim > 0){
      root -> dim_sub_right = right_dim;
    }else {root -> dim_sub_right = 0;}
    #pragma omp task firstprivate(set, left,index_split,ax,depth)
      root -> left = build_tree(set,left,index_split -1, 1-ax,depth+1);
    #pragma omp task firstprivate(set, left,index_split,ax,depth)
      root -> right = build_tree(set, index_split+1, right, 1-ax,depth+1);
    #pragma omp taskwait
    return root;
  }
}

void tree_to_array(tree_node* root, node* vtree, int dim){
  stack_node* stack; stack=NULL;
  int i=0, cond=FALSE;
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
    i++;
    cond = FALSE;
  }
}

void push(stack_node** stack, tree_node *point){
  stack_node *new;
  new = malloc(sizeof(stack_node));
  new -> elem = point;
  new -> next = *stack;
  *stack = new;
}

tree_node *pop(stack_node** stack){
  stack_node *aux;
  tree_node* value;
  aux = *stack;
  value = aux -> elem;
  *stack = (*stack) -> next;
  free(aux);
  return value;
}
