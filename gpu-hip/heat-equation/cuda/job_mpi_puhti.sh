#!/bin/bash
#SBATCH --job-name=HotCuda
#SBATCH --account=project_2000745
#SBATCH --partition=gpu
#SBATCH --reservation=summerschool-gpu
#SBATCH --time=00:05:00
#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --ntasks-per-node=1
#SBATCH --gres=gpu:v100:2

srun heat_cuda
