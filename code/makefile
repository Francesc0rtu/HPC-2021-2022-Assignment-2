

CC = mpicc
CFLAGS  = -fopenmp -std=c99


all: kdtree


kdtree:	main.o utilities.o utilities_omp.o mpi_tree.o omp_tree.o
	$(CC) $(CFLAGS)  main.o utilities.o utilities_omp.o mpi_tree.o omp_tree.o -o  kdtree.x
	$(RM) *.o
main.o:	main.c utilities.h mpi_tree.h
	$(CC) $(CFLAGS) -c  main.c -o main.o

utilities_omp.o: utilities_omp.c utilities.h utilities_omp.h
	$(CC) $(CFLAGS) -c  utilities_omp.c -o utilities_omp.o
utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c  utilities.c -o utilities.o


mpi_tree.o: mpi_tree.c mpi_tree.h utilities.h omp_tree.h utilities_omp.h
	$(CC) $(CFLAGS) -c  mpi_tree.c -o mpi_tree.o

omp_tree.o: omp_tree.c omp_tree.h utilities.h utilities_omp.h
	$(CC) $(CFLAGS) -c  omp_tree.c -o omp_tree.o
clean:
	$(RM) kdtree.x *.o *~ input
