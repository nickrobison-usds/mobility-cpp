#!/bin/bash
#SBATCH -A covid19
#SBATCH -p covid19_burst
#SBATCH -N 2
#SBATCH -n 12
#SBATCH -c 3
#SBATCH -J dwell-time
#SBATCH --mem=300g
#SBATCH -t 4-12:00:00
#SBATCH -o ./%j-dwell-time-output.txt
#SBATCH -e ./%j-dwell-time-error.txt

# Must run from root of project
srun build-python/src/dwell-time/dwell-time --config=slurm/cades.dwell.config.yml