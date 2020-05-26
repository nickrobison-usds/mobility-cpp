#!/bin/bash
#SBATCH -A covid19
#SBATCH -p covid19
#SBATCH -N 3
#SBATCH -n 3
#SBATCH -c 36
#SBATCH -J joiner
#SBATCH --mem=300g
#SBATCH -t 1:00:00
#SBATCH -o ./%j-joiner-output.txt
#SBATCH -e ./%j-joiner-error.txt

srun ../build/src/joiner --input_dir=/lustre/or-hydra/cades-birthright/nickrobison/mobility-analysis/data/safegraph/core --output_file=/lustre/or-hydra/cades-birthright/nickrobison/mobility-analysis/data/reference/joined-locations.parquet --shapefile=/lustre/or-hydra/cades-birthright/nickrobison/mobility-analysis/data/reference/census/block_groups.shp