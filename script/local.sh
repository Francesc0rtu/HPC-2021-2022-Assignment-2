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
  for i in {1..1}
  do
    for j in {1..30}
    do
      export    OMP_NUM_THREADS=${j}
<<<<<<< HEAD
      mpirun -np ${i} ./kdtree.x 10000 > "../output/output_${i}_${j}"
=======
      mpirun -np ${i} ./kdtree.x 100000 > /dev/null #"../output/output_${i}_${j}"
>>>>>>> cbcbf6947490678b4cd91479f7f9295d439f9f88
      cat ../output/time >> ../output/time.csv
    done
  done
fi
rm ../output/time
