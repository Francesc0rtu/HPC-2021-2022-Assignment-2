#include "utilities.h"



void print(data* set, int dim){
  for (size_t i = 0; i < dim; i++) {
    printf("(%d,%d), ", set[i].x, set[i].y);
  }printf("\n");
}

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
    target = (max.x-min.x)/2;
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
    target = (max.y-min.y)/2;
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

void print_ktree(knode* root){
  if(root!=NULL){
    printf("{(%d,%d),%d} ", (root->value).x, (root->value).y, root->AxSplit );
    print_ktree(root->left);
    print_ktree(root->right);
  }
}

void Print_ktree_(knode* root){
  if(root!=NULL){
    printf("(%d,%d), ", (root->value).x, (root->value).y );
    Print_ktree_(root->left);
    Print_ktree_(root->right);
  }
}
