#!/bin/bash
# Environment wrapper for scaling_study.py — uses the local build at bin/bin.
set -e
SW=/home/amorris/sci/shapeworks
source ~/miniconda3/etc/profile.d/conda.sh
conda activate shapeworks
export PATH=$SW/bin/bin:$PATH
export LD_LIBRARY_PATH=$SW/dependencies/install/lib:$SW/bin/bin:$LD_LIBRARY_PATH
export PYTHONPATH=$SW/bin/bin:$PYTHONPATH
for M in $SW/Python/*/; do export PYTHONPATH=$M:$PYTHONPATH; done
cd $SW/Examples/Python
exec python scaling_study.py "$@"
