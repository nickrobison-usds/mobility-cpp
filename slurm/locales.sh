#!/bin/bash

#SBATCH -A covid19
#SBATCH -p covid19_burst
#SBATCH -N 2
#SBATCH -n 4
#SBATCH -c 6
#SBATCH -J locales
#SBATCH --mem=1g
#SBATCH -t 1:00
#SBATCH -o ./%j-locales-output.txt
#SBATCH -e ./%j-locales-error.txt

srun ../build/locales/locales