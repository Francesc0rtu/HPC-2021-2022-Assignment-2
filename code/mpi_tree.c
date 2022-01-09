#include "utilities.h"
#include "omp_tree.h"
#include <unistd.h>

int next_step(int step);
data* resize(data* set, int dim);

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
  displacements[1] = MPI_Aint_diff(displacements[1], base_address);               //
  MPI_Datatype types[2] = { MPI_FLOAT_T, MPI_FLOAT_T};

  MPI_Type_create_struct(2, block_length, displacements, types, &MPI_DATA_TYPE);  // Create the datatype and set the typde available
  MPI_Type_commit(&MPI_DATA_TYPE);

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
  node split_values[num_step];
  int k = num_step, split_index;

  while(step>=1){
   split = 1 - split;

   MPI_Barrier(MPI_COMM_WORLD);

   if(rank%(2*step)==0){                                                     // This one are the sending processors
     if(rank + step < size){
       find_max_min(&max, &min, set, dim);                                // Instead of compute each time I can send min.x=set[split_index].x max.x=max.x and

       split_index = split_and_sort(set, max, min, 0, dim-1, split);
       int new_dim = split_index, send_dim = (dim - new_dim -1);

       k--;
       split_values[k].value = set[split_index];
       split_values[k].AxSplit = 1-split;

       MPI_Send(&send_dim, 1, MPI_INT, rank+step, 0, MPI_COMM_WORLD);
       MPI_Send(set+split_index+1, send_dim, MPI_DATA_TYPE, rank+step,0, MPI_COMM_WORLD);      // Send the data

       set = resize(set, new_dim);
       dim = new_dim;
     }
   }else if(rank%step == 0 && rank!= MASTER){                             // This one are the recv processors
     MPI_Recv(&dim, 1, MPI_INT, rank-step, 0, MPI_COMM_WORLD, &status );      // Receive the dimension of the data
     set = malloc(sizeof(data)*dim);                                            // Allocate the space for the data
     MPI_Recv(set, dim, MPI_DATA_TYPE, rank-step,0, MPI_COMM_WORLD, &status);    // Receive the data
   }
   step = next_step(step);
 }

  MPI_Barrier(MPI_COMM_WORLD);
  // for(int i=0; i<size; i++){
  //   sleep(1);
  //   if(rank==i){
  //     printf("sono %d, e ho %d elementi \n", rank, dim);
  //     print(set,dim);
  //     for(int j=k; j<num_step; j++){
  //       printf("[%d, %d].%d  ",(split_values[j].value).x,(split_values[j].value).y,split_values[j].AxSplit);
  //     }printf("rank=%d \n",rank);
  //   }
  // }

  knode *root;
  if(rank == 3){
    printf(":----------\n ");
    print(set, dim);
    sleep(1);
  #pragma omp parallel
    {
      #pragma omp single
      root = build_omp_tree(set, 0, dim-1, 1-split, 0);
    }
    print_ktree_ascii(root, 0);
    printf("------------------------------ \n");
    knode* array_tree;
    array_tree = tree_to_array(root, dim);
    print_array_knode(array_tree,dim);
  }


}

data* resize(data* set, int dim){
  data* aux;
  aux = malloc(sizeof(data)*dim);
  for(int i=0; i<dim; i++){
    aux[i] = set[i];
  }
  free(set);
  return aux;
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
