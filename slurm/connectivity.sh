#!/bin/bash
#SBATCH -A covid19
#SBATCH -p covid19
#SBATCH -N 10
#SBATCH -n 10
#SBATCH -c 30
#SBATCH -J connectivity
#SBATCH --mem=300g
#SBATCH -t 4-12:00:00
#SBATCH -o ./%j-connectivity-output.txt
#SBATCH -e ./%j-connectivity-error.txt

srun ../build/src/connectivity --data_dir=/lustre/or-hydra/cades-birthright/nickrobison/mobility-analysis/data