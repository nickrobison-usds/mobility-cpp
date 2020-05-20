#!/bin/bash
#SBATCH -A covid19
#SBATCH -p covid19
#SBATCH -N 7
#SBATCH -n 7
#SBATCH -c 30
#SBATCH -J splitter
#SBATCH --mem=300g
#SBATCH -t 1:00:00
#SBATCH -o ./%j-splitter-output.txt
#SBATCH -e ./%j-splitter-error.txt

srun ../build/src/splitter --input_dir=/lustre/or-hydra/cades-birthright/nickrobison/mobility-analysis/data/output/SG-April-weekly-summary.parquet --output_file=/lustre/or-hydra/cades-birthright/nickrobison/mobility-analysis/data/output/SG-April-daily.parquet