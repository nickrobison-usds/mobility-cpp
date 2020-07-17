#!/bin/bash
#SBATCH -A covid
#SBATCH -p regular
#SBATCH -N 20
#SBATCH -n 20
#SBATCH -c 30
#SBATCH -C haswell
#SBATCH -J connectivity
#SBATCH --mem=100
#SBATCH -t 4-12:00:00
#SBATCH -o ./%j-connectivity-output.txt
#SBATCH -e ./%j-connectivity-error.txt

srun ../build/src/connectivity --data_dir=$SCRATCH/mobility-analysis/data/ --np=20 --silent --start_date=2020-07-06