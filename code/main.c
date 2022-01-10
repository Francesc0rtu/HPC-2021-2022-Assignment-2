#include "utilities.h"
#include "mpi_tree.h"

data* init_random_set(int dim);

int main(int argc, char* argv[]){
  int rank,size, provided = 0;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);                  // MPI init for multi-threading hybrid omp
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);                                        // Get rank and size
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int dim = atoi(argv[1]);
  data* set;
  if(rank == 0){
    set = init_random_set(dim);
    print(set, dim);
  }
  node* tree;
  tree = build_mpi_tree(set, dim);

  MPI_Finalize();
}


data* init_random_set(int dim){
  data *aux;
  aux = malloc(sizeof(data)*dim);
  srand(time(NULL));
  for (size_t i = 0; i < dim; i++) {
    aux[i].x = rand() / 1000000;
    aux[i].y = rand() / 1000000;
  }
  return aux;
}
