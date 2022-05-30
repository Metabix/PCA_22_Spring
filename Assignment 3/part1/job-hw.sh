#!/bin/bash
#SBATCH --job-name=lab3
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=mohitpscallps@gmail.com
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --mem-per-cpu=1000mb
#SBATCH -t 00:05:00
#SBATCH -o openMP_4thread
#SBATCH -e myerr
export OMP_NUM_THREADS=4
srun --mpi=pmix_v3 ./part1 60 12 10