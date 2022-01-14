#!/bin/bash
#PBS -l walltime=02:00:00
#PBS -q dssc
#PBS -l nodes=2:ppn=2

cd $PBS_O_WORKDIR
cd ../code
module load openmpi-4.1.1+gnu-9.3.0
make
cp kdtree.x ../output/kdtree.x
cd ../output
export OMP_NUM_THREADS=12
mpirun -np 4 --map-by socket kdtree.x 10000 > output

make clean
git add output time
git commit -m "new output from orfeo"
git push

exit
