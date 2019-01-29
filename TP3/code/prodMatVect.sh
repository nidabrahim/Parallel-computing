#!/bin/bash

# Options de sbatch
#SBATCH --partition=part-etud 	# partition pour tps etudiants
#SBATCH --ntasks=8		# 8 tasks / processus
#SBATCH --cpus-per-task=1	# de 1 thread
#SBATCH --job-name=prodMatVect

# Execution du programme
mpiexec ./prodMatVect
