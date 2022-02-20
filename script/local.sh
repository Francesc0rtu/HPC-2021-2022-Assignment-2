#!/bin/bash

if [ $1 == "--clean" ]; then
  cd ../output
  rm time* output*
else
  cd ../code
  make
  export 		OMP_PROC_BIND=close
  export		MV2_ENABLE_AFFINITY=0 
  export 		OMP_PLACES=cores
  printf '%s\t%s\t%s\t%s\t%s\t%s\n' 'MPI process' 'OMP thread' 'Send MSG,' 'OMP time,' 'Recv msg,' 'total time'  > ../output/time.csv
  for i in {1..1}
  do
    for j in {1..16}
    do
	export OMP_NUM_THREADS=${j}
      ./kdtree.x 1000000 > "../output/output_${i}_${j}"
      cat ../output/time >> ../output/time.csv
    done
  done
fi
rm ../output/time
