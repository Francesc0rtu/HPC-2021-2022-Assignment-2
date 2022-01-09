#include "utilities.h"

int next_step(int step);

node* build_mpi_tree(data* set, int dim){
  int rank,size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Datatype MPI_DATA_TYPE;
  MPI_Status status;
  int block_length[2]={1,1};
  MPI_Aint displacements[2];
  data cell;
  MPI_Aint base_address;
  MPI_Get_address(&cell, &base_address);                                          //Calculate the displacements
  MPI_Get_address(&cell.x, &displacements[0]);                                    //i.e. Calculate the size in bytes of
  MPI_Get_address(&cell.y, &displacements[1]);                                    // each block, in this case the size of MPI_FLOAT_T.
  displacements[0] = MPI_Aint_diff(displacements[0], base_address);               //
  MPI_Datatype types[2] = { MPI_FLOAT_T, MPI_FLOAT_T};

  MPI_Type_create_struct(2, block_length, displacements, types, &MPI_DATA_TYPE);  // Create the datatype and set the typde available
  MPI_Type_commit(&MPI_DATA_TYPE);
  displacements[1] = MPI_Aint_diff(displacements[1], base_address);               //

  node *tree;
  tree = malloc(sizeof(tree)*dim);
  int num_step = 1, step = 1, split = Y;
  if(size > 2){
    do{
      num_step++;
      step = step*2;
    }while(2*step<size);
  }

  data max, min;
  int k = 0, split_index;

  if(rank == 0){
    find_max_min(&max, &min, set, dim);
    printf("max = %d \n", max.x);
    split_index = split_and_sort(set, max, min, 0, dim-1, split);
    print(set,dim);
    printf("index=%d, (%d,%d)\n", split_index, set[split_index].x, set[split_index].y);
  }
  while(step>=1){
   split = 1 - split;

   MPI_Barrier(MPI_COMM_WORLD);

   if(rank%(2*step)==0){                                                     // This one are the sending processors
     if(rank + step < size){
       //invio a rank + step
       if(rank == 0){
     }
     }
   }else if(rank%step == 0 && rank!= MASTER){                             // This one are the recv processors
     //ricevo da rank-step
   }
   step = next_step(step);
 }

}







int next_step(int step){
  if(step == 2){                                                              // Compute the next step
    return 1;
  }
  if(step == 1){
    return - 1;
  }
  return  step = step/2;
}
