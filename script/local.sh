#!/bin/bash

if [ $1 == "--clean" ]; then
  cd ../output
  rm time* output*
else
  cd ../code
  make
  export 		OMP_PLACES=cores
  export 		OMP_PROC_BIND=close
  export		MV2_ENABLE_AFFINITY=0
  printf '%s\t%s\t%s\t%s\t%s\t%s\n' 'MPI process' 'OMP thread' 'Send MSG,' 'OMP time,' 'Recv msg,' 'total time'  > ../output/time.csv
    for j in {1..8}
    do
      export    OMP_NUM_THREADS=${j}
      mpirun -np 1 ./kdtree.x 1000000 #> "../output/output_${i}_${j}"
      cat ../output/time >> ../output/time.csv
    done
fi
rm ../output/time
