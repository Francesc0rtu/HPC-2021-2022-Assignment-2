

CC = mpicc
CFLAGS  = -fopenmp


all: kdtree


kdtree:	main.o utilities.o mpi_tree.o
	$(CC) $(CFLAGS)  main.o utilities.o mpi_tree.o -o  kdtree.x

main.o:	main.c utilities.h main.h
	$(CC) $(CFLAGS) -c  main.c -o main.o

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c  utilities.c -o utilities.o

mpi_tree.o: mpi_tree.c mpi_tree.h
	$(CC) $(CFLAGS) -c  mpi_tree.c -o mpi_tree.o
clean:
	$(RM) kdtree.x *.o *~
