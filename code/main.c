#include "utilities.h"
#include "mpi_tree.h"

data* init_random_set(int dim);

int main(int argc, char* argv[]){
  int rank,size, provided = 0;
  double start_time, end_time;
  MPI_Init(&argc, &argv);                  // MPI init for multi-threading hybrid omp
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);                                        // Get rank and size
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int dim = atoi(argv[1]);
  data* set;
  if(rank == 0){
    set = init_random_set(dim);
    print(set, dim);
  }
  FILE *fptr;
  if(rank == 0){
    fptr = fopen("time", "w");
    fprintf(fptr, "COMPUTATIONAL TIME MPI-OMP 2DTREE:\n");
    fclose(fptr);
  }

  node* tree;

  MPI_Barrier(MPI_COMM_WORLD);
  start_time = MPI_Wtime();

  tree = build_mpi_tree(set, dim);

  MPI_Barrier(MPI_COMM_WORLD);
  end_time = MPI_Wtime() - start_time;

  if(rank == 0){
    fptr = fopen("time", "a");
    fprintf(fptr,"total time = %f \n", end_time);
    fclose(fptr);
    print_tree_ascii(tree, dim, 0);
  }

  MPI_Finalize();
}


data* init_random_set(int dim){
  data *aux;
  aux = malloc(sizeof(data)*dim);
  srand(time(NULL));
  for (size_t i = 0; i < dim; i++) {
    aux[i].x = rand() / (float_t) 1000;
    aux[i].y = rand() / (float_t) 1000;
  }
  return aux;
}
