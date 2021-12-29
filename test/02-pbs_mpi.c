#include<stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sched.h>



int main(int argc, char *argv[]){
  int rank, size;
#pragma omp parallel
  {
    printf("sono il thread %d  on CPU %d\n", omp_get_thread_num(), sched_getcpu());
  }
  return 0;
}
