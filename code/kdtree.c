#include<stdio.h>
#include <stdlib.h>
//#include <mpi.h>
#include <omp.h>
#include<time.h>

///////////////////////////// MACRO /////////////////////////////

#if !defined(DOUBLE_PRECISION)
#define float_t float
#else
#define float_t double
#endif

///////////////////////// STRUCT /////////////////////////////////

typedef struct {
float_t x;
float_t y;
} data_type;

///////////////////// HEADER FUNCTION /////////////////////////////

data_type* initialize_random_dataset_serial(int dim);
void print_data_set(data_type* data, int dim);

/////////////////// MAIN ////////////////////////////////////////

int main(int argc, char *argv[]){
int dim = atoi(argv[1]);                                                        // initialize the dimension of the input
                                                                                // the data set will be a square of dim x dim point
data_type *dataset;                                                             // pointer to the dataset
dataset = initialize_random_dataset_serial(dim);                                // Initialize the dataset
print_data_set(dataset, dim);
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
