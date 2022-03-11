node* build_omp_tree(data* set, int dim, int ax, int depth);
node* build_mpi_tree(data* set, int  dim);
int next_step(int step);
data* resize(data* set, int  dim);
MPI_Datatype create_MPI_type_NODE();
MPI_Datatype create_MPI_type_DATA();
int initialize_step();
