#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>

int cond(int min, int x){
  if(min>x) {return 1;}
  return 0;
}
int dist(int x, int y){
  if(y>=x) return (y-x);
  else return x-y;
}

int find_min(int *v, int* set, int dim, int target){
  int index = v[0];
  for(int i=0; i<dim; i++){
    if(dist(set[v[i]], target) < dist(set[index], target)){
     index = v[i];
    }
  }
  return index;
}


int main(int argc, char* argv[]){
  int n = atoi(argv[1]);
  int v[n];
  srand(time(NULL));
  for(int i = 0; i < n; i++){
    v[i] = rand()% 100;
  }
  for(int i = 0; i<n; i++){
    printf("%d ", v[i]);
  }printf("\n");
  int sum = 0, min=v[n/2], max=v[n/2];
  int left=0, right=n;
  int target=50;
  int index = 0;
  float a,b;
  b = dist(v[index], target);
  int* local;
  int dim;
  #pragma omp parallel private(index) shared(dim, local)
  {
  #pragma omp single
  {
    dim=omp_get_num_threads();
    local = malloc(sizeof(int)*dim);
  }
  #pragma omp parallel for reduction(min:min) reduction(max:max)
  for(int i=0;i<n;i++){
    if(min>v[i]) {min=v[i];}
    if(max<v[i]) {max=v[i];}
  }

  #pragma omp for
    for(int i=left; i<=right; i++){
      if(dist(v[i], target) < b){
       index = i;
       printf("index = %d io sono %d \n", index, omp_get_thread_num());
       b = dist(v[index], target);
      }
    }
    local[omp_get_thread_num()] = index;
  }
  for(int i=0; i<dim; i++){
    printf("%d ", local[i]);
  }printf("/n");
  index = find_min(local, v, dim, target);

  printf(" index=%d, v[index]=%d\n", index, v[index]);
  printf("min=%d, max=%d \n", min,max);
}
