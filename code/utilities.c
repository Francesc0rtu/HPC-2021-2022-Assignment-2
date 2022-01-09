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
  printf("[(%d,%d),%d]\n", (root->value).x, (root->value).y, root->AxSplit);

  // Process left child
  print_ktree_ascii(root->left, space);
}

void print_array_knode(knode* array, int dim){
  for(int i=0; i<dim; i++){
    printf("(%d,%d), ", (array[i].value).x, (array[i].value).y );
  }
}

//////////////////// TO COMPUTE SPLIT AND REORGANIZE DATA ////////

void find_max_min(data* max,data* min, data* set, int dim){
  *max = set[0];
  *min = set[0];

  for(size_t i = 0; i < dim; i++){
    if(set[i].x < min->x){
      min->x = set[i].x;
    }
    if(set[i].x > max->x){
      max->x = set[i].x;
    }
    if(set[i].y < min->y){
      min->y = set[i].y;
    }
    if(set[i].y > max->y){
      max->y = set[i].y;
    }
  }

}

int split_and_sort(data* set, data max, data min, int left, int right, int ax){
  int index = left, dim = (right - left + 1), center = dim/2 + left;
  int k=0, h=right-left;
  float_t target;
  data *aux;
  aux = malloc(sizeof(data)*(right - left + 1));
  if(ax == X){
    target = (max.x-min.x)/2 + min.x;
    // printf("target %d \n", target);
    for(int i=left; i<=right; i++){
      if(dist(set[i].x, target) < dist(set[index].x, target)){
        index = i;
      }
    }
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
    // printf("target %d \n", target);
    for(int i=left; i<=right; i++){
      if(dist(set[i].y, target) < dist(set[index].y, target)){
        index = i;
      }
    }
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


/////////////////// TREE TO ARRAY AND VICEVERSA /////////////////////

knode* tree_to_array(knode* root, int dim){
  knode *array;
  array = malloc(sizeof(knode)*dim);
  int i=0;
  map_to_array(array,root,dim,0);
  // print_array_knode(array,dim);
  return array;
}

void map_to_array(knode* array, knode* root, int dim, int i){
  if(i<dim){
    if(root!=NULL){
      array[i] = *root;
      map_to_array(array, root->left, dim, i+1);
      map_to_array(array, root->right, dim, i+1);
    }
  }
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
