#!/bin/bash
qsub 2dtree_gpu.sh
qsub 2dtree_mpi_omp.sh
qsub 2dtree_perf_small.sh
