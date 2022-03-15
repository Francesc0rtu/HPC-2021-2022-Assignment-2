#!/bin/bash
#PBS -l walltime=02:00:00
#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=48

cd $PBS_O_WORKDIR
if [ $1 == "--clean" ]; then
  cd ../output
  rm output*
else
  cd ../code
  module load openmpi-4.1.1+gnu-9.3.0
  make

  export    OMP_NUM_THREADS=12
  export 		OMP_PLACES=sockets
  export 		OMP_PROC_BIND=close
  export		MV2_ENABLE_AFFINITY=0

  printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' 'MPI,' 'OMP,' 'Send MSG,' 'OMP time,' 'Array,' 'Recv msg,' 'total time,' 'N,'  > ../output/time_weak.csv
  y=1
  for i in {1..5}
  do
    for j in {1..30}
    do
      export    OMP_NUM_THREADS=${j}
      ((x=y*j*1000000))
      mpirun -np ${y} --map-by socket --mca btl ^openib kdtree.x ${x}
      cat ../output/time >> ../output/time_weak.csv
      echo ${x} >> ../output/time_weak.csv
      rm ../output/time
    done
   ((y=2*y))
  done
  export    OMP_NUM_THREADS=2
  for j in {1..30}
  do
    ((x=2*j*1000000))
    mpirun -np ${j} --map-by socket --mca btl ^openib kdtree.x ${x}
    cat ../output/time >> ../output/time_weak.csv
    echo ${x} >> ../output/time_weak.csv
    rm ../output/time
  done
fi
exit
