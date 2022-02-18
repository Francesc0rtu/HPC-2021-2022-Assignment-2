#include "utilities.h"
#include "mpi_tree.h"
#include "omp_tree.h"
#include <unistd.h>




node* build_mpi_tree(data* set, int dim){
  // This function divide the data-set among the MPI process, then each proces builds its tree
  // calling the build_omp_tree routine. Then the MPI process send their subtree to the master.

  int rank,size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Status status;
  MPI_Datatype MPI_DATA = create_MPI_type_DATA();             // Create the MPI type for the struct data
  MPI_Datatype MPI_NODE = create_MPI_type_NODE();             // Create the MPI type for the struct node
  double mpi_time, omp_time;
  FILE *fptr;


  data max, min;                                                  // Declare and initialize basic variable for the splitting part
  int num_step, step, split = Y,split_index, depth = 0;
  initialize_step(&step, &num_step);
  node split_values[num_step];
  int k = num_step;
  printf("k=%d\n",k);
  ////////////////////////////////////////// DATA DISPLACEMENT WITH TREE-BASED METHOD //////////////////////
  // In this portion of code the data are divided between all the MPI processes with a tree method.       //
  // Each time that a process have to send to his child-process,                                          //
  // it divide the data in two part and save the split value (that will be a node of the final tree).     //
  // In this way, at the end of this routine each process has received an array of a subset of data-set.  //
  // This is an example with 11 processors:                                                               //
  //                                                                                                      //
  //                                              0                                                       //
  //                                             / \                                                      //
  //                                            /   \                                                     //
  //                                           /     \                                                    //
  //                                          /       \                                                   //
  //                                         /         \                                                  //
  //                                        0           8          --- step==4                            //
  //                                       / \           \                                                //
  //                                      /   \           \                                               //
  //                                     /     \           \                                              //
  //                                    /       \           \                                             //
  //                                   0        4           8        --- step==2                          //
  //                                  / \      / \         / \                                            //
  //                                 /   \    /   \       /   \                                           //
  //                                0     2   4    6      8   10      --- step==1                         //
  //                               / \   / \ / \  / \    / \   |                                          //
  //                              0  1  2  3 4  5 6  7  8  9  10                                          //
  //                                                                                                      //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  MPI_Barrier(MPI_COMM_WORLD);
  mpi_time = MPI_Wtime();

  while(step>=1){
   split = 1 - split;
   if(rank%(2*step)==0){                             // This one are the sending processors
     if(rank + step < size){                        // The rcv process must exist
       find_max_min(&max, &min, set, dim);          // Find max and min in the set, both for x and y

       split_index = split_and_sort(set, max, min, 0, dim-1, split);            // Find the index of the splitting value of the set
       int new_dim = split_index, send_dim = (dim - new_dim -1);

       split_values[k].value = set[split_index];                                // Save the split value to re-build the tree in the next routine
       split_values[k].AxSplit = split;
       split_values[k].depth = depth ;
       k--;

       MPI_Send(&send_dim, 1, MPI_INT, rank+step, 0, MPI_COMM_WORLD);            // Send the dimension of the portion of data to send to the other process
       MPI_Send(set+split_index+1, send_dim, MPI_DATA, rank+step,0, MPI_COMM_WORLD);      // Send the data

       set = resize(set, new_dim);                                              // Resize the data after have sended the right part of the data
       dim = new_dim;
     }
   }else if(rank%step == 0 && rank!= MASTER){                             // This one are the recv processors
     MPI_Recv(&dim, 1, MPI_INT, rank-step, 0, MPI_COMM_WORLD, &status );      // Receive the dimension of the data
     set = malloc(sizeof(data)*dim);                                            // Allocate the space for the data
     MPI_Recv(set, dim, MPI_DATA, rank-step,0, MPI_COMM_WORLD, &status);    // Receive the data
   }
   step = next_step(step);                              // Compute the next step
   depth++;                                             // At each step the depth become larger
 }
 MPI_Barrier(MPI_COMM_WORLD);
 mpi_time = MPI_Wtime() - mpi_time;
 if(rank == 0){
   fptr = fopen("../output/time", "a");
   fprintf(fptr,"\t%f,", mpi_time);
   fclose(fptr);
 }
 //////////////////////////////////// SINGLE PROCESS BUIDLING TREE ////////////////////////////////////////////
  node* tree;
  omp_time = MPI_Wtime();
  #pragma omp parallel
  {
    printf("Sono il processo %d e thread %d \n", rank, omp_get_thread_num());
    #pragma omp single
    tree=build_omp_tree(set, 0,dim-1,1-split,depth);                          // Each MPI process build its tree in a multi-threading way
  }
  MPI_Barrier(MPI_COMM_WORLD);
  omp_time = MPI_Wtime() - omp_time;
  if(rank == 0){
    fptr = fopen("../output/time", "a");
    fprintf(fptr,"\t%f,", omp_time);
    fclose(fptr);
  }
  // for(int i=0; i<size; i++){
  //   sleep(1);
  //   if(rank==i){
  //     printf("///////////////  %d ////////////// \n",rank);
  //     print_tree_ascii(tree, 0, 0);
  //     // print_tree(tree,dim);
  //   }
  // }

  //////////////////////////////////////// SEND TREES TO THE MASTER //////////////////////////////////////////////
  // At each step half of the MPI processes involved send their tree the other half processes:                  //
  // these ones will merge their tree with the tree recived, and then the step is incremented.                  //
  // This is an example with 8 MPI processes.                                                                   //
  //                                                                                                            //
  //                             0     1     2     3     4     5     6     7                                    //
  //                               \   /       \   /       \   /       \   /                                    //
  //                                 0           2           4           6          ---- Step=1                 //
  //                                  \         /             \         /                                       //
  //                                   \       /               \       /                                        //
  //                                    \     /                 \     /                                         //
  //                                       0                       4                ---- step=2                 //
  //                                        \                     /                                             //
  //                                         \                   /                                              //
  //                                          \                 /                                               //
  //                                           \               /                                                //
  //                                            \             /                                                 //
  //                                                  0                           ---- step=4                   //
  //                                                                                                            //
  //                                                                                                            //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  MPI_Barrier(MPI_COMM_WORLD);
  mpi_time = MPI_Wtime();
  int check = FALSE, rcv_dim;
  step=1; k++;
  while(step<size){
    if(rank%(2*step)==0){                 // This are the receving process
      if(rank + step < size){
        MPI_Recv(&rcv_dim, 1, MPI_INT, rank+step, 0, MPI_COMM_WORLD, &status);   //Recive the dimension of the tree to merge

        node *rcv_array, *merge_array;                                          // Allocate memory for the rcv tree for the merged one
        rcv_array = malloc(sizeof(node)*rcv_dim);
        merge_array = malloc(sizeof(node)*(rcv_dim+dim+1));

        MPI_Recv(rcv_array,rcv_dim,MPI_NODE,rank+step,0,MPI_COMM_WORLD,&status);  // Receive the tree
        merge_array->value = split_values[k].value;    // Assign the father values
        merge_array[0].left = 1;
        merge_array[0].right = dim + 1;
        k++;
        tree = expand(tree, rcv_array, merge_array, dim, rcv_dim);              // Merge the father with the left-subtree and the right-subtree
        dim = rcv_dim+dim+1;                                                    // Update the dimension of the tree
      }
    }else if(check == FALSE){    //mando a rank-step
      MPI_Send(&dim,1,MPI_INT, rank-step, 0, MPI_COMM_WORLD);                 //Send the dimension of the tree to send
      MPI_Send(tree, dim, MPI_NODE, rank-step, 0, MPI_COMM_WORLD);            // Send the tree
      free(tree);                                                             // Free the memory: now my work is done!!
      check=TRUE;                                                             // My work is done, I will do nothing anymore!
    }
    step = step*2;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  mpi_time = MPI_Wtime() - mpi_time;

  if(rank == 0){
    fptr = fopen("../output/time", "a");
    fprintf(fptr,"\t%f,", mpi_time);
    fclose(fptr);
  }
  return tree;
}



data* resize(data* set, int dim){
  // This function deleting all the elements in the array set that are an index
  // greater than dim-1. Than return the pointer to an array with the correct new size.
  set = realloc(set, dim*sizeof(data));

  // data* aux;
  // aux = malloc(sizeof(data)*dim);
  //
  // #pragma omp parallel
  // {
  //   #pragma omp for
  //   for(int i=0; i<dim; i++){
  //     aux[i] = set[i];
  //   }
  // }
  // free(set);

  return set;
}

void initialize_step(int* step, int *num_step){
  // Basic operation to compute the first value of step and count the number of step
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  *num_step = 1;
  *step = 1;
  if(size > 2){
    do{
      *num_step++;
      *step = *step*2;
    }while(2*(*step)<size);
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

MPI_Datatype create_MPI_type_NODE(){
  MPI_Datatype MPI_NODE, MPI_DATA = create_MPI_type_DATA();
  int block_length_[5]={1,1,1,1,1};
  MPI_Aint displacements_[5];
  node cell_;
  MPI_Aint base_address_;
  MPI_Get_address(&cell_, &base_address_);                                          //Calculate the displacements
  MPI_Get_address(&cell_.value, &displacements_[0]);                                    //i.e. Calculate the size in bytes of
  MPI_Get_address(&cell_.AxSplit, &displacements_[1]);                                    // each block, in this case the size of MPI_FLOAT_T.
  MPI_Get_address(&cell_.depth, &displacements_[2]);
  MPI_Get_address(&cell_.left, &displacements_[3]);                                    // each block, in this case the size of MPI_FLOAT_T.
  MPI_Get_address(&cell_.right, &displacements_[4]);
  for(int i=0; i<5; i++){
    displacements_[i] = MPI_Aint_diff(displacements_[i], base_address_);               //
  }
  MPI_Datatype types_[5] = { MPI_DATA, MPI_INT, MPI_INT,MPI_INT,MPI_INT};
  MPI_Type_create_struct(5, block_length_, displacements_, types_, &MPI_NODE);  // Create the datatype and set the typde available
  MPI_Type_commit(&MPI_NODE);
  return MPI_NODE;
}
