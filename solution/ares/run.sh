#!/bin/bash

#SBATCH --job-name=membrana
#SBATCH --time=00:05:00
#SBATCH --account=plgar2024-cpu
#SBATCH --partition=plgrid-testing
#SBATCH --mem=2G
#SBATCH --output=out.csv
#SBATCH --error=out.err
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=12

module load openmpi
module load python
mpicc -lm -ldl -o $HOME/AR/pro/main $HOME/AR/pro/main.c -D SIDE_LENGTH=23 -D ITERATIONS=650
mpiexec -np 12 $HOME/AR/pro/main
python $HOME/AR/pro/heatmap.py  $HOME/AR/pro/out.csv
