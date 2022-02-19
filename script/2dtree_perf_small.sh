#!/bin/bash
#PBS -l walltime=02:00:00
#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=24

cd $PBS_O_WORKDIR
if [ $1 == "--clean" ]; then
  cd ../output
  rm time* output*
else
  cd ../code
  module load openmpi-4.1.1+gnu-9.3.0
  make
  printf '%s\t%s\t%s\t%s\t%s\t%s\n' 'MPI process' 'OMP thread' 'Send MSG,' 'OMP time,' 'Recv msg,' 'total time'  > ../output/time_perf_small.csv
  export    OMP_NUM_THREADS=2
  export 		OMP_PLACES=cores
  export 		OMP_PROC_BIND=close
  export		MV2_ENABLE_AFFINITY=0


  for i in  {1..12}
  do
      mpirun -np ${i} --map-by socket --mca btl ^openib kdtree.x 1000000 > "../output/output_${i}_${j}"
      cat ../output/time >> ../output/time_perf_small.csv
  done

  for i in {1..12}
  do
    export    OMP_NUM_THREADS=${i}
    mpirun -np 2 --map-by socket --mca btl ^openib kdtree.x 1000000 > "../output/output_${i}_${j}"
    cat ../output/time >> ../output/time_perf_small.csv
  done

fi

rm ../output/time
exit
