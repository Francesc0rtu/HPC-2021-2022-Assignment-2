#include<stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <omp.h>
#include <sched.h>


// Primo programma hybrid openMP openMPI.
// Il programma inizializza i processi MPI e per ogni processo crea una paralel region:
// Ogni threads in parallel region stampa il numero del proprio thread, il rank del MPI process e il numero fisico della cpu_num
// sul quale è.


int main(int argc, char *argv[]){
  int rank, size;
  int nthreads, provided;
  MPI_Status status;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int messaggio=-777;
  if(rank == 0){
    messaggio = 2022;
  }
  MPI_Send(&messaggio, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
  MPI_Recv(&messaggio, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  #pragma omp parallel
    {
    int cpu_num = sched_getcpu();
    printf("ciao, io sono il thread %d con rank_MPI %d on CPU:%d \t il mio messaggio è %d \n", omp_get_thread_num(), rank, cpu_num, messaggio);

    }

  MPI_Finalize();
  return 0;
}
