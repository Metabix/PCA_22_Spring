#!/bin/bash
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=mohitpscallps@gmail.com
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=8
#SBATCH --ntasks=8
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1000mb
#SBATCH -t 00:05:00
#SBATCH -o outfile
#SBATCH -e errfile
srun --mpi=pmix_v2 ./cmtbonebe 100 5 4 4 4 2 2 2