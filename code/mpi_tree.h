
node* build_mpi_tree(data* set, int dim);
int next_step(int step);
data* resize(data* set, int dim);
MPI_Datatype create_MPI_type_NODE();
MPI_Datatype create_MPI_type_DATA();
void initialize_step(int* step, int *num_step);
