#!/bin/bash
#PBS -l walltime=02:00:00
#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=24

cd $PBS_O_WORKDIR
if [ $1 == "--clean" ]; then
  cd ../output
  rm output*
else
  cd ../code
  module load openmpi-4.1.1+gnu-9.3.0
  make

  export    OMP_NUM_THREADS=12
  export 		OMP_PLACES=cores
  export 		OMP_PROC_BIND=close
  export		MV2_ENABLE_AFFINITY=0

  printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' 'MPI,' 'OMP,' 'Send MSG,' 'OMP time,' 'Array,' 'Recv msg,' 'total time,' 'N,'  > ../output/time_weak.csv
  y=1
  for j in {1..10}
  do
    ((y=y*10))
    ((x=y*10000))
    mpirun -np 4 --map-by socket --mca btl ^openib kdtree.x ${x}
    cat ../output/time >> ../output/time_weak.csv
    echo ${x} >> ../output/time_weak.csv
  done
fi
rm ../output/time
exit
