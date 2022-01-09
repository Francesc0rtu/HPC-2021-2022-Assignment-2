

CC = mpicc
CFLAGS  = -fopenmp


all: kdtree


kdtree:	main.o utilities.o mpi_tree.o omp_tree.o
	$(CC) $(CFLAGS)  main.o utilities.o mpi_tree.o omp_tree.o -o  kdtree.x
	$(RM)	*.o

main.o:	main.c utilities.h mpi_tree.h
	$(CC) $(CFLAGS) -c  main.c -o main.o

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c  utilities.c -o utilities.o

mpi_tree.o: mpi_tree.c omp_tree.h utilities.h
	$(CC) $(CFLAGS) -c  mpi_tree.c -o mpi_tree.o

omp_tree.o: omp_tree.c utilities.h
	$(CC) $(CFLAGS) -c  omp_tree.c -o omp_tree.o
clean:
	$(RM) kdtree.x *.o *~
