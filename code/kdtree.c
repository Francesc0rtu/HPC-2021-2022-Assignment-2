#include<stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include<time.h>

///////////////////////////// MACRO /////////////////////////////

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

#define MASTER 0
///////////////////////// STRUCT /////////////////////////////////

typedef struct {
float_t x;
float_t y;
} data_type;

///////////////////// HEADER FUNCTION /////////////////////////////

data_type* initialize_random_dataset_serial(int dim);
void print_data_set(data_type* data, int dim);
void parallel_sort(data_type* dataset, int dim);

/////////////////// MAIN ////////////////////////////////////////

int main(int argc, char *argv[]){

  int my_rank,size, provided=0;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);                  // MPI init for multi-threading hybrid omp
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);                                        // Get rank and size
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int dim = atoi(argv[1]);                                                       // initialize the dimension of the input                                                                                // the data set will be a square of dim x dim point
  data_type *dataset;                                                            // pointer to the dataset
  if(my_rank == MASTER){                                                              // Master processor init the data set
    dataset = initialize_random_dataset_serial(dim);                             // Initialize the dataset
    print_data_set(dataset, dim);
  }
  parallel_sort(dataset, dim);                                             // Sort in parallel the dataset
  MPI_Finalize();
}

/////////////////////// FUNCTIONS ////////////////////////////////


data_type* initialize_random_dataset_serial(int dim){                           // Allocates and initialize the data set
  data_type* data;                                                              // with random numbers.
  data = malloc(sizeof(data_type)*dim);
  srand(time(NULL));
  for(int i=0; i<dim; i++){
    data[i].x = (double)rand()/100000;
    data[i].y = (double)rand()/100000;
  }
  return data;
}

void print_data_set(data_type* data, int dim){                                  // Print on standard output an array
  for(int i=0; i<dim; i++){                                                     //of type data_type and size dim
    printf("(%f,%f)",data[i].x, data[i].y);
  }
  printf("\n");
}

void parallel_sort(data_type* dataset, int dim){
/// Create a MPI data type for send at each processor a part of the dataset/////
  MPI_Datatype MPI_DATA_TYPE;

  int block_length[2]={1,1};
  MPI_Aint displacements[2];
  data_type cell;
  MPI_Aint base_address;
  MPI_Get_address(&cell, &base_address);                                          //Calculate the displacements
  MPI_Get_address(&cell.x, &displacements[0]);                                    //i.e. Calculate the size in bytes of
  MPI_Get_address(&cell.y, &displacements[1]);                                    // each block, in this case MPI_FLOAT_T
  displacements[0] = MPI_Aint_diff(displacements[0], base_address);               //
  displacements[1] = MPI_Aint_diff(displacements[1], base_address);               //
  MPI_Datatype types[2] = { MPI_FLOAT_T, MPI_FLOAT_T};

  MPI_Type_create_struct(2, block_length, displacements, types, &MPI_DATA_TYPE);  // Create the datatype and set ad available
  MPI_Type_commit(&MPI_DATA_TYPE);                                                //

/// Compute the dimension of each part to send and sending with MPI_Scatterv ///
  int my_rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);                                        // Get rank and size
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  data_type* buffer_rcv;
  int count_send[size], count_recv, displacement[size];
  for(int i=0; i< size - 1; i++){                                                // Set the number of elements to send of each processors
    count_send[i] = (int) dim/size;
    displacement[i] = count_send[i]*i;
  }
  count_send[size-1] = (int) dim/size + dim%size ;
  displacement[size-1] = count_send[size-2]*(size-1);
  count_recv = count_send[my_rank];
  buffer_rcv = malloc(sizeof(data_type)*count_send[my_rank]);                    // Allocate the memory for the rcv buffer of each mpi process

  for(int i=0; i<size; i++){
    printf(" (%d / %d ) ", displacement[i], count_send[i]);
  } printf("\n");

   MPI_Scatterv(dataset, count_send, displacement, MPI_DATA_TYPE, buffer_rcv, count_recv, MPI_DATA_TYPE, MASTER, MPI_COMM_WORLD);   // Send at each mpi process a part of the datset

   if(my_rank == 3){
     print_data_set(buffer_rcv, count_recv);
   }

}
