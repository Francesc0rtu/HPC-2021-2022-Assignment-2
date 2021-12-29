#include<stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sched.h>



int main(int argc, char *argv[]){
  int rank, size;
omp_set_num_threads(24);
#pragma omp parallel
  {
#pragma omp single
	{printf("ahahahah sono solo \n");}
    printf("sono il thread %d  on CPU %d\n", omp_get_thread_num(), sched_getcpu());
  }
  return 0;
}
