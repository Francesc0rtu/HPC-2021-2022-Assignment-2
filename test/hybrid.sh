#!/bin/bash
#PBS -l nodes=2:ppn=1
#PBS -l walltime=01:00:00
#PBS -q dssc

module load openmpi-4.1.1+gnu-9.3.0
export		MV2_ENABLE_AFFINITY=0
export		OMP_NUM_THREADS=4
mpicc -fopenmp hybrid_OMPI_OMP.c -o hybrid_OMPI_OMP.x
mpirun -bind-to	user:0+1+2+3,4+5+6+7 -np 2 hybrid_OMPI_OMP.x


exit
