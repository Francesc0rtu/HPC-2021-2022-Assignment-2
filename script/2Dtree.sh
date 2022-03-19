#!/bin/bash
#PBS -l walltime=02:00:00
#PBS -q dssc
#PBS -l nodes=2:ppn=24

cd $PBS_O_WORKDIR
if [ $1 == "--clean" ]; then
  cd ../output
  rm output*
else
  module load openmpi-4.1.1+gnu-9.3.0
  make

  export 		OMP_PLACES=cores
  export 		OMP_PROC_BIND=true
  

  printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\n' 'MPI,' 'OMP,' 'Send MSG,' 'OMP time,' 'Array,' 'Recv msg,' 'total time'  > ../output/time_thin_FULL_CORE.csv

  for i in  {1..24}
  do
    for j in {1..24}
    do
      export    OMP_NUM_THREADS=${j}
      mpirun -np ${i} --map-by socket --mca btl ^openib kdtree.x 10000000 
      cat time >> ../output/time_thin_FULLCORE.csv
      printf '\n' >> ../output/time_thin_FULL_CORE.csv
    done
  done
fi
rm time
exit
