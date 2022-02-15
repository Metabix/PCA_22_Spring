#!/bin/bash
#SBATCH --job-name=mc_MPI
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=mohitpscallps@gmail.com
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=32
#SBATCH --ntasks-per-node=32
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1000mb
#SBATCH -t 00:05:00
#SBATCH -o SendRecvR32N10000
#SBATCH -e myerr
srun --mpi=pmix_v3 -n 32 ./mc  -10 10 10000