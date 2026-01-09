#!/bin/bash
# Run exact check for all DeepSSM test configurations
# Usage: ./run_exact_check.sh save|verify

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR="${DATA:-$(dirname "$SCRIPT_DIR")/data}"

if [ "$1" != "save" ] && [ "$1" != "verify" ]; then
    echo "Usage: $0 save|verify"
    echo "  save   - Save baseline values (run before refactoring)"
    echo "  verify - Verify against saved values (run after refactoring)"
    exit 1
fi

MODE="$1"
CONFIGS="default tl_net fine_tune tl_net_fine_tune"

# Unzip test data if not already extracted
if [ ! -d "${DATA_DIR}/deepssm" ]; then
    unzip -q "${DATA_DIR}/deepssm_test_data.zip" -d "${DATA_DIR}/deepssm"
fi

cd "${DATA_DIR}/deepssm/projects"

for config in $CONFIGS; do
    echo "========================================"
    echo "Running $config..."
    echo "========================================"

    rm -rf deepssm groomed *_particles
    shapeworks deepssm --name ${config}.swproj --all

    # Run exact check with config-specific file
    python "${SCRIPT_DIR}/verify_deepssm_results.py" . \
        --exact_check "$MODE" \
        --baseline_file "exact_check_${config}.txt"

    echo ""
done

echo "========================================"
echo "All configurations: $MODE complete!"
echo "========================================"
