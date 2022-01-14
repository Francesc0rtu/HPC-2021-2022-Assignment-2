#include "utilities.h"
#include "mpi_tree.h"
#include <mpi.h>
#include <omp.h>

void init_random_set(int dim);

int main(int argc, char* argv[]){
  int rank,size, provided = 0;
  double start_time, end_time;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);                  // MPI init for multi-threading hybrid omp
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);                                        // Get rank and size
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  FILE *fptr, *input;

  int dim = atoi(argv[1]);
  data* set;

  if(rank == 0){
    init_random_set(dim);
    set = malloc(sizeof(data)*dim);
    input = fopen("input", "r");
    for(int i=0; i<dim; i++){
      fscanf(input, "%f %f", &set[i].x, &set[i].y);
    }
    fclose(input);
  }

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


void init_random_set(int dim){
  float_t x;
  float_t y;
  FILE *out;
  out =fopen("input", "w" );
  srand(time(NULL));
  for (size_t i = 0; i < dim; i++) {
    x = rand() / (float_t) 1000;
    y = rand() / (float_t) 1000;
    fprintf(out, "%f %f\n", x,y);
  }
  fclose(out);
}
