#!/bin/bash
# Test DeepSSM with TL-DeepSSM network enabled
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Unzip test data if not already extracted
if [ ! -d "${DATA}/deepssm" ]; then
    unzip -q "${DATA}/deepssm_test_data.zip" -d "${DATA}/deepssm"
fi

cd "${DATA}/deepssm/projects"
rm -rf deepssm groomed *_particles
shapeworks deepssm --name tl_net.swproj --all

# Verify results
python3 "${SCRIPT_DIR}/verify_deepssm_results.py" "${DATA}/deepssm/projects"
