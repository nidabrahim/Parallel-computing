#!/bin/bash

# submit_hello_mpi.sh fichier pour l'execution de hello_mpi
# Options de sbatch
#SBATCH --partition=part-etud 	# partition pour tps etudiants
#SBATCH --ntasks=8		# 8 tasks / processus
#SBATCH --cpus-per-task=1	# de 1 thread
#SBATCH --job-name=testMPI

# Execution du programme
mpiexec ./hello

