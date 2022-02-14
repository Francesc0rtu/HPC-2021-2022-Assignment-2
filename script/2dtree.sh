#!/bin/bash
#PBS -l walltime=02:00:00
#PBS -q dssc
#PBS -l nodes=2:ppn=24

cd $PBS_O_WORKDIR
if [ $1 == "--clean" ]; then
  cd ../output
  rm time* output*
else
  cd ../code
  module load openmpi-4.1.1+gnu-9.3.0
  make
  export    OMP_NUM_THREADS=12
  export 		OMP_PLACES=cores
  export 		OMP_PROC_BIND=close
  export		MV2_ENABLE_AFFINITY=0

  printf '%s\t%s\t%s\t%s\t%s\t%s\n' 'MPI process' 'OMP thread' 'Send MSG,' 'OMP time,' 'Recv msg,' 'total time'  > ../output/time.csv

  for i in  {1..4}
  do
    for j in {1..12}
    do
      export    OMP_NUM_THREADS=${j}
      mpirun -np ${i} --map-by socket --mca btl ^openib kdtree.x 100000000 > "../output/output_${i}_${j}"
      cat ../output/time >> ../output/time.csv
    done
  done
fi
rm ../output/time
exit
