#!/bin/bash
#PBS -l walltime=02:00:00
#PBS -q dssc
#PBS -WMail_Users=francescortu@live.it
#PBS -m -ae
#PBS -l nodes=2:ppn=2

cd $PBS_O_WORKDIR
module load openmpi-
make
mpirun -np 4 kdtree.x 10000 > output
git add output
git commit -m "new output from orfeo"
git push

exit
