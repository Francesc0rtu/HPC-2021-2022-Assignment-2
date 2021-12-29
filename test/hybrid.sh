#!/bin/bash
#PBS -l nodes=2:ppn=1
#PBS -l walltime=01:00:00
#PBS -q dssc

cd $PBS_O_WORKDIR
module load openmpi-4.1.1+gnu-9.3.0
export		MV2_ENABLE_AFFINITY=0
export		OMP_NUM_THREADS=24
mpicc -fopenmp hybrid_OMPI_OMP.c -o hybrid_OMPI_OMP.x
mpirun -np 2 --report-bindings --map-by node hybrid_OMPI_OMP.x 


exit
