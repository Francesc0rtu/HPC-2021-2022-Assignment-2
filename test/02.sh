#!/bin/bash
#PBS -l nodes=2:ppn=3
#PBS -l walltime=01:00:00
#PBS -q dssc

cd $PBS_O_WORKDIR
module load openmpi-4.1.1+gnu-9.3.0
mpicc -fopenmp 02-pbs_mpi.c -o 02.x
mpirun -np 6 --report-bindings --map-by node 02.x 2>error > output
exit
