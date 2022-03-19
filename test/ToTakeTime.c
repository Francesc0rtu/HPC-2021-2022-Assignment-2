#include<omp.h>
#include <mpi.h>
#include<stdlib.h>
#include <stdio.h>
#include <time.h>

#if !defined(DOUBLE_PRECISION)
#define float_t float
#else
#define float_t double
#endif


#if !defined(DOUBLE_PRECISION)
#define MPI_FLOAT_T MPI_FLOAT
#else
#define MPI_FLOAT_T MPI_DOUBLE
#endif


typedef struct {
float_t x;
float_t y;
} data;

MPI_Datatype create_MPI_type_DATA(){
  MPI_Datatype MPI_DATA;
  int block_length[2]={1,1};
  MPI_Aint displacements[2];
  data cell;
  MPI_Aint base_address;
  MPI_Get_address(&cell, &base_address);                                          //Calculate the displacements
  MPI_Get_address(&cell.x, &displacements[0]);                                    //i.e. Calculate the size in bytes of
  MPI_Get_address(&cell.y, &displacements[1]);                                    // each block, in this case the size of MPI_FLOAT_T.
  displacements[0] = MPI_Aint_diff(displacements[0], base_address);               //
  displacements[1] = MPI_Aint_diff(displacements[1], base_address);               //
  MPI_Datatype types[2] = { MPI_FLOAT_T, MPI_FLOAT_T};
  MPI_Type_create_struct(2, block_length, displacements, types, &MPI_DATA);  // Create the datatype and set the typde available
  MPI_Type_commit(&MPI_DATA);
  return MPI_DATA;
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


int main(int argc, char* argv[]){

int rank, size, dim; 
MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD,&rank);
dim = atoll(argv[1]);
data* set;
if(rank==0)
    set = init_random_set(dim);
if(rank==1)
    set = malloc(sizeof(data)*dim);
MPI_Status status;
MPI_Datatype MPI_DATA = create_MPI_type_DATA();             // Create the MPI type for the struct data
double time=MPI_Wtime();
if(rank == 0){
    MPI_Send(set, dim, MPI_DATA, 1,0, MPI_COMM_WORLD);
}
if(rank ==1){
    MPI_Recv(set, dim, MPI_DATA, 0,0, MPI_COMM_WORLD,&status);
}
time = MPI_Wtime() - time;
printf("time to send: %f \n", time);

MPI_Finalize();

}