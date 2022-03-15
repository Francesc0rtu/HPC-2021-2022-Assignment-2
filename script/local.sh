#!/bin/bash

if [ $1 == "--clean" ]; then
  cd ../output
  rm  output*
else
  cd ../code
  make
  export 		OMP_PROC_BIND=close
  export		MV2_ENABLE_AFFINITY=0 
  export 		OMP_PLACES=sockets
   printf '%s\t%s\t%s\t%s\t%s\t%s\n' 'MPI process' 'OMP thread' 'Send MSG,' 'OMP time,' 'Recv msg,' 'total time'  > ../output/time.csv
  export OMP_NUM_THREADS=1
  mpirun -np 1  --mca btl ^openib ./kdtree.x 10000000 
  cat ../output/time >> ../output/time.csv
  x=1
  for i in {1..4}
  do
    ((x=2*x))
	  export OMP_NUM_THREADS=1
     mpirun -np ${x}  --mca btl ^openib ./kdtree.x 10000000 
     cat ../output/time >> ../output/time.csv
  done
fi
rm ../output/time
