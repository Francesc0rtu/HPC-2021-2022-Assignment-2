

CC = mpicc
CFLAGS  = -fopenmp -std=c99

all: kdtree
debug: CFLAGS += -DDEBUG
debug: kdtree
double: CFLAGS += -DOUBLE_PRECISION
double: kdtree


kdtree:	main.o tree.o  mpi_tree.o omp_tree.o
	$(CC) $(CFLAGS)  main.o tree.o mpi_tree.o omp_tree.o -o  kdtree.x
	$(RM) *.o
main.o:	main.c tree.h mpi_tree.h
	$(CC) $(CFLAGS) -c  main.c -o main.o

tree.o: tree.c tree.h
	$(CC) $(CFLAGS) -c  tree.c -o tree.o

mpi_tree.o: mpi_tree.c mpi_tree.h tree.h
	$(CC) $(CFLAGS) -c  mpi_tree.c -o mpi_tree.o

omp_tree.o: omp_tree.c omp_tree.h tree.h 
	$(CC) $(CFLAGS) -c  omp_tree.c -o omp_tree.o
clean:
	$(RM) kdtree.x *.o *~ input
