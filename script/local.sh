#!/bin/bash

cd ../code
make
for i in {1..4}
do
  mpirun -np ${i} ./kdtree.x 1000 > "../output/output_${i}"
  touch "../output/time_${i}"
  mv ../output/time "../output/time_${i}"
done
