#!/bin/bash
# Test DeepSSM with fine tuning enabled
set -e

# Prevent PyTorch/OpenMP deadlock on macOS and Windows
export OMP_NUM_THREADS=1

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Unzip test data if not already extracted
if [ ! -d "${DATA}/deepssm" ]; then
    unzip -q "${DATA}/deepssm_test_data.zip" -d "${DATA}/deepssm"
fi

cd "${DATA}/deepssm/projects"
rm -rf deepssm groomed *_particles
shapeworks deepssm --name fine_tune.swproj --all --aug_processes 1

# Verify results
python3 "${SCRIPT_DIR}/verify_deepssm_results.py" "${DATA}/deepssm/projects"
