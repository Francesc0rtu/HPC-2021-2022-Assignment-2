#include "utilities.h"
#include "omp_tree.h"
#include <unistd.h>

int next_step(int step);
data* resize(data* set, int dim);
node* expand(node* array_tree, node* rcv_array, node* merge_array, int dim,int rcv_dim);

node* build_mpi_tree(data* set, int dim){
  int rank,size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Datatype MPI_DATA;
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
  MPI_Type_create_struct(2, block_length, displacements, types, &MPI_DATA);  // Create the datatype and set the typde available
  MPI_Type_commit(&MPI_DATA);

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
  int k = num_step, split_index, depth = 0;

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
       split_values[k].AxSplit = split;
       split_values[k].depth = depth ;

       MPI_Send(&send_dim, 1, MPI_INT, rank+step, 0, MPI_COMM_WORLD);
       MPI_Send(set+split_index+1, send_dim, MPI_DATA, rank+step,0, MPI_COMM_WORLD);      // Send the data

       set = resize(set, new_dim);
       dim = new_dim;
     }
   }else if(rank%step == 0 && rank!= MASTER){                             // This one are the recv processors
     MPI_Recv(&dim, 1, MPI_INT, rank-step, 0, MPI_COMM_WORLD, &status );      // Receive the dimension of the data
     set = malloc(sizeof(data)*dim);                                            // Allocate the space for the data
     MPI_Recv(set, dim, MPI_DATA, rank-step,0, MPI_COMM_WORLD, &status);    // Receive the data
   }
   step = next_step(step);
   depth++;
 }

  printf("depth = %d \n", depth);
  knode *root;

  printf(":----------\n ");
  print(set, dim);
  sleep(1);
  node* array_tree;
  array_tree = malloc(sizeof(node)*dim);
  int i=-1;
  #pragma omp parallel
    {printf("lalalalallala %d \n", i);
      #pragma omp single
      // root = build_omp_tree(set, 0, dim-1, 1-split, depth);
      i=build_omp_array_tree(set,array_tree, 0,dim-1,1-split,depth,&i);
    }
  // array_tree = tree_to_array(root, dim);

  for(int i=0; i<size; i++){
    sleep(1);
    if(rank==i){
      printf("///////////////  %d ////////////// \n",rank);
      print_ktree_ascii(root, 0);
      print_array_node(array_tree,dim);
    }
  }



  MPI_Datatype MPI_NODE;
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




  int check = FALSE, rcv_dim;
  step=1;
  while(step<size){
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank%(2*step)==0){
      if(rank + step < size){    //ricevo da rank+step
        node *rcv_array, *merge_array;
        MPI_Recv(&rcv_dim, 1, MPI_INT, rank+step, 0, MPI_COMM_WORLD, &status);
        // printf("sono %d e ricevo %d elmenti da %d \n", rank,rcv_dim,rank+step);
        printf("il mio split value e (%d,%d) depth=%d \n", (split_values[k].value).x, (split_values[k].value).y, split_values[k].depth );
        sleep(1);
        rcv_array = malloc(sizeof(node)*rcv_dim);
        merge_array = malloc(sizeof(node)*(rcv_dim+dim+1));
        MPI_Recv(rcv_array,rcv_dim,MPI_NODE,rank+step,0,MPI_COMM_WORLD,&status);
        merge_array[0] = split_values[k];
        merge_array[0].left = 1;
        merge_array[0].right = dim;
        k++;
        array_tree = expand(array_tree, rcv_array, merge_array, dim, rcv_dim);
        sleep(1);
        dim = rcv_dim+dim+1;
        // print_array_node(array_tree,dim);
      }
    }else if(check == FALSE){    //mando a rank-step
      // printf("sono %d e mando %d elmenti a %d \n", rank,dim,rank-step);
      sleep(1);
      MPI_Send(&dim,1,MPI_INT, rank-step, 0, MPI_COMM_WORLD);                 //Send the dimension
      MPI_Send(array_tree, dim, MPI_NODE, rank-step, 0, MPI_COMM_WORLD);
      free(array_tree);
      ////////// c`e' da liberare l'albero
      // free_tree(root);
      check=TRUE;
    }
    step = step*2;
  }

  if(rank == 0){
    printf("dim %d \n", dim);
    print_array_node(array_tree,dim);
  }



}

node* expand(node* array_tree, node* rcv_array, node* merge_array, int dim,int rcv_dim){
  #pragma omp parallel
  {
    #pragma omp parallel for
    for(int i=0; i<dim; i++){
      merge_array[i+1] = array_tree[i];
      if(merge_array[i+1].left != -1){
        merge_array[i+1].left = merge_array[i+1].left + 1;
      }
      if(merge_array[i+1].right != -1){
        merge_array[i+1].right = merge_array[i+1].right + 1;
      }
    }
    #pragma omp parallel for
    for(int i=0; i<rcv_dim; i++){
      merge_array[i+dim+1] = rcv_array[i];
      if(merge_array[i+dim+1].left != -1){
        merge_array[i+dim+1].left = merge_array[i+dim+1].left + dim + 1;
      }
      if(merge_array[i+dim+1].right != -1){
        merge_array[i+dim+1].right = merge_array[i+dim+1].right + dim + 1;
      }
    }
  }
  free(array_tree);
  free(rcv_array);
  return merge_array;
}

data* resize(data* set, int dim){
  data* aux;
  aux = malloc(sizeof(data)*dim);

  #pragma omp parallel
  {
    #pragma omp parallel for
    for(int i=0; i<dim; i++){
      aux[i] = set[i];
    }
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
