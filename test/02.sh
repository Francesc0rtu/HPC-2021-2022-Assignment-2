#!/bin/bash
#PBS -l nodes=2:ppn=2
#PBS -l walltime=01:00:00
#PBS -q dssc

cat $PBS_NODEFILE 
cd $PBS_O_WORKDIR
module load openmpi-4.1.1+gnu-9.3.0
gcc -fopenmp 02-pbs_mpi.c -o 02.x
./02.x 2>error > output
exit
