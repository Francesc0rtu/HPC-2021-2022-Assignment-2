#include "tree.h"
#include "main.h"
#include <string.h>

int main(int argc, char* argv[]){
  int rank,size, provided = 0;
  struct timespec ts;
  double start_time, end_time;
  MPI_Init(&argc, &argv);                         // MPI init 
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);          // Get rank and size
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  FILE *fptr, *input;
  int  dim = atoll(argv[1]);
  data* set;

  if(rank == 0){                              // Init a random set 
    set=init_random_set(dim);
  }

  if(rank == 0){                              // Print in the output file the number of MPI process and OMP threads
    #pragma omp parallel
    {
      #pragma omp single
      {
        fptr = fopen("../output/time", "w");
        fprintf(fptr, "%d,%d,", size, omp_get_num_threads());
        fclose(fptr);
      }
    }
  }

  node* tree;

  MPI_Barrier(MPI_COMM_WORLD);            
  start_time = CPU_TIME;

  tree = build_mpi_tree(set, dim);

  MPI_Barrier(MPI_COMM_WORLD);
  end_time = CPU_TIME - start_time;

  if(rank == 0){
    fptr = fopen("../output/time", "a");
    fprintf(fptr,"\t%f,\n", end_time);
    fclose(fptr);

    if(dim < 100){
      print_tree_ascii(tree, 0, 0);
      print_tree(tree,dim);
    }

    if(argv[2] != NULL && strcmp(argv[2],"--print") == 0)
      print_to_file(tree,dim);

    free(set);
    free(tree);
  }
  MPI_Finalize();
}


data* init_random_set(int  dim){
  float_t x;
  float_t y;
  data* set; 
  set = (data*) malloc(dim*sizeof(data));
  srand(time(NULL));
  for (int  i = 0; i < dim; i++) {
    x = rand() / (float_t) 1000;
    y = rand() / (float_t) 1000;
    set[i].x = x;
    set[i].y = y; 
  }
  return set;
}
