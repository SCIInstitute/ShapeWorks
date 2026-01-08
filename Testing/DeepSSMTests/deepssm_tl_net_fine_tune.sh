#!/bin/bash
# Test DeepSSM with both TL-DeepSSM and fine tuning enabled
set -e

# Unzip test data if not already extracted
if [ ! -d "${DATA}/deepssm" ]; then
    unzip -q "${DATA}/deepssm_test_data.zip" -d "${DATA}/deepssm"
fi

cd "${DATA}/deepssm/projects"
rm -rf deepssm groomed *_particles
shapeworks deepssm --name tl_net_fine_tune.swproj --all
