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

pip install -q --upgrade pip
pip install -q pandas
pip install -q numpy
pip install -q matplotlib
pip install -q seaborn
pip install -q imageio
pip install -q tqdm

# TODO: tutaj sobie ustawcie swoją ścieżkę:
export P="$HOME/ar/AR-project/solution/ares"

mpicc -lm -ldl -o "$P/main" "$P/main.c" -D SIDE_LENGTH=23 -D ITERATIONS=650
mpiexec -np 12 "$P/main"
python "$P/heatmap.py"  "$P/out.csv"