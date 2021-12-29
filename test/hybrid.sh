#!/bin/bash
#PBS -l nodes=2:ppn=24
#PBS -l walltime=01:00:00
#PBS -q dssc

cd $PBS_O_WORKDIR
module load openmpi-4.1.1+gnu-9.3.0
export 		OMP_PLACES=cores
export 		OMP_PROC_BIND=close
export		MV2_ENABLE_AFFINITY=0
export		OMP_NUM_THREADS=24
mpicc -fopenmp hybrid_OMPI_OMP.c -o hybrid_OMPI_OMP.x
mpirun -np 4 --report-bindings --map-by socket hybrid_OMPI_OMP.x 2>error > output 


exit
