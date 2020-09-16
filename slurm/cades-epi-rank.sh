#!/bin/bash
#SBATCH -A covid19
#SBATCH -p covid19_burst
#SBATCH -N 7
#SBATCH -n 7
#SBATCH -c 33
#SBATCH -J epi-rank
#SBATCH --mem=300g
#SBATCH -t 4-12:00:00
#SBATCH -o ./%j-epi-rank-output.txt
#SBATCH -e ./%j-epi-rank-error.txt

srun ../build/src/epi-rank/epi-rank --config=cades.epi.config.yml