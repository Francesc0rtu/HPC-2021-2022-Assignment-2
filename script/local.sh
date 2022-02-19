#!/bin/bash

if [ $1 == "--clean" ]; then
  cd ../output
  rm time* output*
else
  cd ../code
  make
  export 		OMP_PLACES=cores
  printf '%s\t%s\t%s\t%s\t%s\t%s\n' 'MPI process' 'OMP thread' 'Send MSG,' 'OMP time,' 'Recv msg,' 'total time'  > ../output/time.csv
  for i in {1..1}
  do
    for j in {1..30}
    do
      export    OMP_NUM_THREADS=${j}
      mpirun -np ${i} --report-bindings ./kdtree.x 100000 > "../output/output_${i}_${j}"
      cat ../output/time >> ../output/time.csv
    done
  done
fi
rm ../output/time
