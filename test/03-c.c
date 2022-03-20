#include<stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sched.h>

int main(){
  
  #pragma omp parallel
  {
  printf("ciao, io sono il thread %d  on CPU:%d  \n", omp_get_thread_num(), sched_getcpu());
  }

}