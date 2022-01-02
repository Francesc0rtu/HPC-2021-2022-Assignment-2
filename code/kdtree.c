#include<stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include<time.h>

///////////////// note-idee ///////////////////////////////////////////////////////////////
// L'idea principale è di dividere il data-set in due vettori, uno per x e
// uno per le y. Ordinare questi due vettori e procedere come se fosse uno solo, rompendo
// la corrispondenza 1-1 tra indice e x, y. Ovvero se i punti sono  (x_assis[i], y_assis[i])
// rompo la corrispondenza unendoli.

///////////////////////////////////////////////////////////////////////////////////////////

///////////////////// READ ME PROVVISIORIO //////////////////////////////////////////////
// Il programma prende in input la dimensione del dataset.


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
#define TRUE 1
#define FALSE 0
///////////////////////// STRUCT /////////////////////////////////

typedef struct {
float_t x;
float_t y;
} data_type;


///////////////////// HEADER FUNCTION /////////////////////////////

data_type* initialize_random_dataset_serial(int dim);
void print_data_set(data_type* data, int dim);
void parallel_sort(data_type* dataset, int dim, data_type** data_x, data_type** data_y);
data_type* merge_x(data_type* data_1,int dim_1, data_type* data_2, int dim_2);
data_type* merge_y(data_type* data_1,int dim_1, data_type* data_2, int dim_2);
void multi_thread_mergesort_y(data_type* data, int left, int right);
void multi_thread_mergesort_x(data_type* data, int left, int right);
void merge_array_x(data_type* data, int left, int center, int right);
void merge_array_y(data_type* data, int left, int center, int right);
/////////////////// MAIN ////////////////////////////////////////

int main(int argc, char *argv[]){

  int my_rank,size, provided=0;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);                  // MPI init for multi-threading hybrid omp
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);                                        // Get rank and size
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int dim = atoi(argv[1]);                                                       // initialize the dimension of the input                                                                                // the data set will be a square of dim x dim point
  data_type *dataset,  *data_x, *data_y;                                                            // pointer to the dataset
  if(my_rank == MASTER){                                                              // Master processor init the data set
    dataset = initialize_random_dataset_serial(dim);                             // Initialize the dataset
    print_data_set(dataset, dim);
  }
  parallel_sort(dataset, dim, &data_x, &data_y);                                             // Sort in parallel the dataset
  if(my_rank == MASTER){
    print_data_set(data_x, dim);
  }
  MPI_Finalize();
}

/////////////////////// FUNCTIONS ////////////////////////////////


data_type* initialize_random_dataset_serial(int dim){                           // Allocates and initialize the data set
  data_type* data;                                                              // with random numbers.
  data = malloc(sizeof(data_type)*dim);
  srand(time(NULL));
  for(int i=0; i<dim; i++){
    data[i].x = (float_t)rand()/100000;
    data[i].y = (float_t)rand()/100000;
  }
  return data;
}

void print_data_set(data_type* data, int dim){                                  // Print on standard output an array
  for(int i=0; i<dim; i++){                                                     //of type data_type and size dim
    printf("(%f,%f)",data[i].x, data[i].y);
  }
  printf("\n");
}


//////////////////////////////////// parallel_sort ////////////////////////////////////////////////
// This function take in input the dataset and the dimension, obv only in the master processors
// there is dataset allocated. Then, after declare a new MPI data type for the struct that I am using
// for the dataset, send to all the others MPI processors a part of the data set and they will return
// to the master their part sorted
/////////////////////////////////////////////////////////////////////////////////////////////////

void parallel_sort(data_type* dataset, int dim, data_type** data_x, data_type** data_y){
/// Create a MPI data type for send at each processor a part of the dataset/////
  MPI_Datatype MPI_DATA_TYPE;
  MPI_Status status;
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

  data_type* data_partial_x, *data_partial_y;
  int count_send[size], displacement[size];
  for(int i=0; i< size - 1; i++){                                                // Set the number of elements to send of each processors
    count_send[i] = (int) dim/size;
    displacement[i] = count_send[i]*i;
  }
  count_send[size-1] = (int) dim/size + dim%size ;
  displacement[size-1] = count_send[size-2]*(size-1);
  data_partial_x = malloc(sizeof(data_type)*count_send[my_rank]);                    // Allocate the memory for the rcv buffer of each mpi process
  data_partial_y = malloc(sizeof(data_type)*count_send[my_rank]);


   MPI_Scatterv(dataset, count_send, displacement, MPI_DATA_TYPE, data_partial_x, count_send[my_rank], MPI_DATA_TYPE, MASTER, MPI_COMM_WORLD);   // Send at each mpi process a part of the datset

   for(int i=0; i<count_send[my_rank]; i++){
     data_partial_y[i] = data_partial_x[i];
   }

  multi_thread_mergesort_x(data_partial_x,0, count_send[my_rank]-1);
  multi_thread_mergesort_y(data_partial_y,0, count_send[my_rank]-1);


  int step=1;
  int check = FALSE;
  while(step<size){
    MPI_Barrier(MPI_COMM_WORLD);
    if(my_rank%(2*step)==0){
      if(my_rank + step < size){
        data_type* other_x, *other_y;
        MPI_Recv(&count_send[my_rank+step],1, MPI_INT,my_rank+step,0,MPI_COMM_WORLD,&status );

        other_x = malloc(sizeof(data_type)*count_send[my_rank+step]);
        other_y = malloc(sizeof(data_type)*count_send[my_rank+step]);

        MPI_Recv(other_x, count_send[my_rank+step], MPI_DATA_TYPE, my_rank+step, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(other_y, count_send[my_rank+step], MPI_DATA_TYPE, my_rank+step, 0, MPI_COMM_WORLD, &status);

        data_partial_x = merge_x(data_partial_x, count_send[my_rank], other_x, count_send[my_rank+step]);
        data_partial_y = merge_y(data_partial_y, count_send[my_rank], other_y, count_send[my_rank+step]);

        count_send[my_rank] = count_send[my_rank]+count_send[my_rank+step];
      }
    }else if(check == FALSE){
      MPI_Send(&count_send[my_rank], 1, MPI_INT, my_rank-step, 0, MPI_COMM_WORLD);
      MPI_Send(data_partial_x, count_send[my_rank],MPI_DATA_TYPE, my_rank-step,0, MPI_COMM_WORLD);
      MPI_Send(data_partial_y, count_send[my_rank],MPI_DATA_TYPE, my_rank-step,0, MPI_COMM_WORLD);
      check = TRUE;
    }
    step = step*2;
  }
  if(my_rank != MASTER){
      free(data_partial_x);
      free(data_partial_y);
  }
  *data_x = data_partial_x;
  *data_y = data_partial_y;


}

data_type* merge_x(data_type* data_1,int dim_1, data_type* data_2, int dim_2){
  data_type *data_merged;
  int dim_merged = dim_1 + dim_2 ;
  data_merged = malloc(sizeof(data_type)*dim_merged);
  int i=0,j=0,k=0;

  while(i<dim_1 && j<dim_2 ){
    if(data_1[i].x <=data_2[j].x){
      data_merged[k] = data_1[i];
      i++;
    }else{
      data_merged[k] = data_2[j];
      j++;
    }
    k++;
  }
  while(i<dim_1){
    (data_merged[k]) = (data_1[i]);
    i++; k++;
  }
  while(j<dim_2){
    (data_merged[k]) = (data_2[j]);
    j++; k++;
  }
  free(data_1);
  free(data_2);
  return data_merged;
}

data_type* merge_y(data_type* data_1,int dim_1, data_type* data_2, int dim_2){
  data_type *data_merged;
  int dim_merged = dim_1 + dim_2 ;
  data_merged = malloc(sizeof(data_type)*dim_merged);
  int i=0,j=0,k=0;

  while(i<dim_1 && j<dim_2 ){
    if(data_1[i].y <= data_2[j].y){
      data_merged[k] = data_1[i];
      i++;
    }else{
      data_merged[k] = data_2[j];
      j++;
    }
    k++;
  }
  while(i<dim_1){
    data_merged[k] = data_1[i];
    i++; k++;
  }
  while(j<dim_2){
    data_merged[k] = data_2[j];
    j++; k++;
  }
  free(data_1);
  free(data_2);
  return data_merged;
}

void multi_thread_mergesort_x(data_type* data, int left, int right){
  data_type* result;
  int center=(left+right)/2;
  if(left<right){
  multi_thread_mergesort_x(data, left, center);
  multi_thread_mergesort_x(data, center+1, right);
  merge_array_x(data, left,center,right);
  }
}
void multi_thread_mergesort_y(data_type* data, int left, int right){
  data_type* result;
  int center=(left+right)/2;
  if(left<right){
  multi_thread_mergesort_y(data, left, center);
  multi_thread_mergesort_y(data, center+1, right);
  merge_array_y(data, left,center,right);
  }
}

void merge_array_y(data_type* data, int left, int center, int right){
  int i=left, j=center+1, k=0;
  data_type aux[right-left+1];
  while(i <= center && j <= right){
    if(data[i].y <= data[j].y){
      aux[k] = data[i];
      i++;
    }else{
      aux[k] = data[j];
      j++;
    }
    k++;
  }
  while(i <= center){
    aux[k] = data[i];
    i++; k++;
  }
  while(j <= right){
    aux[k] = data[j];
    j++; k++;
  }
 for(int h=left; h<=right; h++){
   data[h] = aux[h-left];
 }
}

void merge_array_x(data_type* data, int left, int center, int right){
  int i=left, j=center+1, k=0;
  data_type aux[right-left+1];
  while(i <= center && j <= right){
    if(data[i].x <= data[j].x){
      aux[k] = data[i];
      i++;
    }else{
      aux[k]= data[j];
      j++;
    }
    k++;
  }
  while(i <= center){
    aux[k]= data[i];
    i++; k++;
  }
  while(j <= right){
    aux[k]= data[j];
    j++; k++;
  }
 for(int h=left; h<=right; h++){
   data[h] = aux[h-left];
 }
}
