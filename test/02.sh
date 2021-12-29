#!/bin/bash
#PBS -l nodes=2:ppn=3
#PBS -l walltime=01:00:00
#PBS -q dssc

cd $PBS_O_WORKDIR
module load openmpi-4.1.1+gnu-9.3.0
<<<<<<< HEAD
mpicc -fopenmp 02-pbs_mpi.c -o 02.x
mpirun -np 6 --report-bindings --map-by node 02.x 2>error > output
=======
mpicc 02-pbs_mpi.c -o 02.x
mpirun -np 8 --use-hwthread-cpus --report-bindings --map-by node 02.x 2>error > output
>>>>>>> 3960164835c50f63d9c33faf35b03af2cf6cfe9d


exit
