#!/bin/bash
mpirun -n 2 ./nbody-parallel.exe --threads=4 --particles=10 --iterations=100
