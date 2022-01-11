#include "utilities.h"
#define COUNT 10


/////////////////// PRINT FUNCTIONS ////////////////////

void print(data* set, int dim){
  for (size_t i = 0; i < dim; i++) {
    printf("(%d,%d), ", set[i].x, set[i].y);
  }printf("\n");
}

void print_ktree(knode* root){
  if(root!=NULL){
    printf("{(%d,%d),%d, %d} ", (root->value).x, (root->value).y, root->AxSplit, root->dep );
    print_ktree(root->left);
    print_ktree(root->right);
  }
}

void print_ktree_ascii(knode *root, int space){
  // Base case
  if (root == NULL)
      return;

  // Increase distance between levels
  space += COUNT;

  // Process right child first
  print_ktree_ascii(root->right, space);

  // Print current node after space
  // count
  printf("\n");
  for (int i = COUNT; i < space; i++)
      printf(" ");
  printf("[(%d,%d),%d,%d]\n", (root->value).x, (root->value).y, root->AxSplit, root->dep);

  // Process left child
  print_ktree_ascii(root->left, space);
}

void print_array_node(node* array, int dim){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("------------%d-----------\n",rank);
  for(int i=0; i<dim; i++){
    printf("(%d,%d), ", (array[i].value).x, (array[i].value).y );
  }printf("\n");
}

//////////////////// TO COMPUTE SPLIT AND REORGANIZE DATA ////////

void find_max_min(data* max,data* min, data* set, int dim){
  float_t max_x = set[0].x , max_y = set[0].y, min_x = set[0].x , min_y = set[0].y;
  #pragma omp parallel
  {
    #pragma omp parallel for reduction(min:min_x, min_y) reduction(max:max_x,max_y)
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

void swap(data* x, data* y){
  data tmp;
  tmp = *x;
  *x = *y;
  *y = tmp;
}

float_t dist(float_t x, float_t y){
  if(y>=x) return (y-x);
  else return x-y;
}

int find_split_index(data* set, float_t target, int left, int right, int ax){
  int index = left;
  int* local_index;
  int dim;
  if(ax == X){
    float_t x = dist(set[index].x, target);
    #pragma omp parallel shared(dim, local_index)
    {
      #pragma omp single
      {
        dim=omp_get_num_threads();
        local_index = malloc(sizeof(int)*dim);
      }
      #pragma omp parallel for
      for(int i=left; i<=right; i++){
        if(dist(set[i].x, target) < x){
         index = i;
         x = dist(set[index].x, target);
        }
      }
      local_index[omp_get_thread_num()] = index;
    }

    index = local_index[0];
    for(int i=0; i<dim; i++){
      if(dist(set[local_index[i]].x, target) < dist(set[index].x, target)){
        index = local_index[i];
      }
    }
  }
  if(ax == Y){
    float_t x = dist(set[index].x, target);
    #pragma omp parallel shared(dim, local_index)
    {
      #pragma omp single
      {
        dim=omp_get_num_threads();
        local_index = malloc(sizeof(int)*dim);
      }
      #pragma omp parallel for
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
  return index;
}

int split_and_sort(data* set, data max, data min, int left, int right, int ax){
  int index = left, dim = (right - left + 1), center = dim/2 + left;
  int k=0, h=right-left;
  float_t target;
  data *aux;
  aux = malloc(sizeof(data)*(right - left + 1));
  if(ax == X){
    target = (max.x-min.x)/2 + min.x;
    index = find_split_index(set, target, left, right, ax);
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

  if(ax == Y){
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

/////////////////// TREE TO ARRAY AND VICEVERSA /////////////////////

node* tree_to_array(knode* root, int dim){////////////// SERIAL /////////////7
  node *array;
  array = malloc(sizeof(node)*dim);
  int i=0;
  i=map_to_array(array,root,dim,i);
  // print_array_knode(array,dim);
  return array;
}

int map_to_array(node* array, knode* root, int dim, int i){

  if(i < dim){
    if(root!=NULL){
      array[i].value = root->value;
      array[i].AxSplit = root->AxSplit;
      array[i].depth = root->dep;
      i++;
      if(root->left != NULL){

     // #pragma omp task firstprivate(array, root, dim)
      i = map_to_array(array, root->left, dim, i);
      }
      if(root->right != NULL){
     // #pragma omp task firstprivate(array, root, dim)
      i =  map_to_array(array, root->right, dim, i);
      }

    }
  }
  return i;
}

knode* array_to_tree(knode* array, int dim){
  knode* root;
  int i;
  map_to_tree(array, root, dim, i);
}

void map_to_tree(knode* array, knode* root, int dim, int i){

}

// void merge_array_tree(knode* merge, knode* subleft, knode *subright, int dimlh, int dimrh){
//
// }
