#!/bin/bash
#PBS -l walltime=02:00:00
#PBS -q dssc
#PBS -l nodes=2:ppn=24

cd $PBS_O_WORKDIR
cd ../code
module load openmpi-4.1.1+gnu-9.3.0
make
export    OMP_NUM_THREADS=12
export 		OMP_PLACES=cores
export 		OMP_PROC_BIND=close
export		MV2_ENABLE_AFFINITY=0


for i in  {1..4}
do
  mpirun -np ${i} --map-by socket --mca btl ^openib kdtree.x 100000000 > "../output/output_${i}"
  touch "../output/time_${i}"
  mv ../output/time "../output/time_${i}"
done


exit
