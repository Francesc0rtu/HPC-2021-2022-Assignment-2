# Parallel 2dtree

## Compile
To compile do:
```bash
$ make
```
It is needed OpenMPI and MPI.
This command produce the executable file named `kdtree.x`.

In order to use `double` instead `float` compile with:
```
make double
```



## Run
To run with M MPI processes and N threads use the command:
```bash
export OMP_NUM_THREADS= <N>
mpirun -np <M> ./kdtree <PUT-HERE-SIZE-OF-THE-PROBLEM>
```

For example to run with 4 MPI processes, 9 threads and $10^8$ points do:
```bash
export OMP_NUM_THREADS= 9
mpirun -np 4  ./kdtree 100000000
```

## Output
The program will produce a text file `time` where you can find the time taken in the different part of the program.

If the size is $<100$ the program will print the tree in ASCII on standard output.

If you want to generate a file with the tree saved as csv files add `--print` as arguments after the size of the problem:
```bash
mpirun -np 4  ./kdtree 100000000 --print
```
___WARNING___  : This may take a lot of time and the file produced could be VERY large.

## Structure of the source code
| Files | content |
|--------- | ------------- |
| `tree.c tree.h` |  This files contain all the libraries, macro, structures and functions used from all the other files|
| `main.c main.h` | Main of the program and creation of the data-set |
|`mpi_tree.c mpi_tree.h` | Functions to divide the work from the master to all MPI processes and re-build back the tree to the master.
| `omp_tree.c omp_tree.h` | Functions to build the tree using OMP and convert the tree to an array
