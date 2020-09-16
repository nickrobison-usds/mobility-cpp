#!/bin/bash
#SBATCH -A covid19
#SBATCH -p covid19_burst
#SBATCH -N 10
#SBATCH -n 10
#SBATCH -c 30
#SBATCH -J centrality
#SBATCH --mem=300g
#SBATCH -t 4-12:00:00
#SBATCH -o ./%j-centrality-output.txt
#SBATCH -e ./%j-centrality-error.txt