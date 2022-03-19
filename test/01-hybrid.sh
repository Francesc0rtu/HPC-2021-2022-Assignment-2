#!/bin/bash
#PBS -l nodes=1:ppn=6
#PBS -l walltime=01:00:00
#PBS -q dssc_gpu

cd $PBS_O_WORKDIR
module load openmpi-4.1.1+gnu-9.3.0
export 		OMP_PLACES=sockets
export 		OMP_PROC_BIND=close
export		MV2_ENABLE_AFFINITY=0
export		OMP_NUM_THREADS=6
mpicc -fopenmp 01-hybrid_message_passing.c -o a.out.x
mpirun -np 4 --report-bindings --map-by node a.out.x 2>error > output 


exit
