#!/bin/bash -1
#SBATCH --time=00:15:00
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --constraint=inca12
#SBATCH --account=kurse

echo "tsmm2:"
srun --export"=OMP_NUM_THREADS=1" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm2
srun --export"=OMP_NUM_THREADS=2" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm2
srun --export"=OMP_NUM_THREADS=3" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm2
srun --export"=OMP_NUM_THREADS=4" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm2
srun --export"=OMP_NUM_THREADS=5" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm2
srun --export"=OMP_NUM_THREADS=6" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm2
srun --export"=OMP_NUM_THREADS=12" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm2

echo "tsmm4:"
srun --export"=OMP_NUM_THREADS=1" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm4
srun --export"=OMP_NUM_THREADS=2" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm4
srun --export"=OMP_NUM_THREADS=3" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm4
srun --export"=OMP_NUM_THREADS=4" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm4
srun --export"=OMP_NUM_THREADS=5" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm4
srun --export"=OMP_NUM_THREADS=6" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm4
srun --export"=OMP_NUM_THREADS=12" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm4

echo "tsmm8:"
srun --export"=OMP_NUM_THREADS=1" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm8
srun --export"=OMP_NUM_THREADS=2" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm8
srun --export"=OMP_NUM_THREADS=3" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm8
srun --export"=OMP_NUM_THREADS=4" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm8
srun --export"=OMP_NUM_THREADS=5" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm8
srun --export"=OMP_NUM_THREADS=6" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm8
srun --export"=OMP_NUM_THREADS=12" -n 1 /scratch/performance_engineering_WS1819/likwid/bin/likwid-pin -c 0-11 ./tsmm8