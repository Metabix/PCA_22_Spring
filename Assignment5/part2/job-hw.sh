#!/bin/bash
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=mohitpscallps@gmail.com
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1000mb
#SBATCH -t 00:05:00
#SBATCH -o outfile
#SBATCH -e errfile
srun --mpi=pmix_v2 ./is.B.1