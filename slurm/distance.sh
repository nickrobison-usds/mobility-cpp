#!/bin/bash
#SBATCH -A covid19
#SBATCH -p covid19
#SBATCH -N 1
#SBATCH -n 1
#SBATCH -c 36
#SBATCH -J splitter
#SBATCH --mem=300g
#SBATCH -t 12:00:00
#SBATCH -o ./%j-distance-output.txt
#SBATCH -e ./%j-distance-error.txt

srun ../build/src/distance --input_file=/lustre/or-hydra/cades-birthright/nickrobison/mobility-analysis/data/reference/census/block_groups.shp --output_file=/lustre/or-hydra/cades-birthright/nickrobison/mobility-analysis/data/reference/census/cbg-distance.parquet