
# Parallel 2dtree

This repository contained the parallel implementation of a 2d-tree produced for the Assignment 2 of FHPC course 2021-2022 @ DSSC, Units.


## Description of the problem
Kd-trees are a data structures presented originally by Friedman, Bentley and Finkel in 1977
to
represent a set of k-dimensional data in order to make them efficiently searchable.

In spite of its age, or more likely thanks to it and to the large amount of research and improvements
accumulated in time, kd-trees are still a good pragmatical choice to perform knn (k-nearest
neighbours) operations in many cases.

In this assignment you are required to write a parallel code that builds a kd-tree for k=2. 

You must
implement both the MPI and the OpenMP version.
In order to simplify the task, the following 2 assumptions hold:
- the data set, and hence the related kd-tree, can be assumed immutable, i.e. you can neglect
the insertion/deletion operations;
- the data points can be assumed to be homogeneously distributed in all the k dimensions.

You find a formal introduction
2
to kd-trees in the paper “The k-d tree data structure and
a proof for
neighborhood computation in expected logarithmic time”.

To have more information about the task visit : https://github.com/Foundations-of-HPC/Foundations_of_HPC_2021/tree/main/Assignment2




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


## Structure of the repository
In the branch `main` there is an implementatation of the tree using only arrays. However, this implementation has better performance.