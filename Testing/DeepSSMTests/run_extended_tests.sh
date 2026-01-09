#!/bin/bash
# Run extended DeepSSM tests on a directory of projects
#
# Usage: ./run_extended_tests.sh <base_dir> [save|verify|relaxed] [project]
#
# Arguments:
#   base_dir  - Directory containing project subdirectories
#   mode      - save: save baseline values
#               verify: verify against saved baselines
#               relaxed: run with loose tolerance (default)
#   project   - Optional: run only this project (default: all)
#
# Examples:
#   ./run_extended_tests.sh /path/to/projects              # Run all with relaxed check
#   ./run_extended_tests.sh /path/to/projects save         # Save baselines for all
#   ./run_extended_tests.sh /path/to/projects verify       # Verify all against baselines
#   ./run_extended_tests.sh /path/to/projects save femur   # Save baseline for femur only
#
# Directory structure:
#   base_dir/
#     project1/
#       *.swproj
#       femur/  (or other data)
#     project2/
#       *.swproj
#       ...

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

usage() {
    echo "Usage: $0 <base_dir> [save|verify|relaxed] [project]"
    echo ""
    echo "Arguments:"
    echo "  base_dir  - Directory containing project subdirectories"
    echo "  mode      - save|verify|relaxed (default: relaxed)"
    echo "  project   - Run only this project (default: all)"
    echo ""
    echo "Examples:"
    echo "  $0 /path/to/projects"
    echo "  $0 /path/to/projects save"
    echo "  $0 /path/to/projects verify"
    echo "  $0 /path/to/projects save femur"
}

if [ $# -lt 1 ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    usage
    exit 0
fi

BASE_DIR="$1"
MODE="${2:-relaxed}"
PROJECT="${3:-all}"

if [ ! -d "$BASE_DIR" ]; then
    echo "Error: Directory not found: $BASE_DIR"
    exit 1
fi

if [ "$MODE" != "save" ] && [ "$MODE" != "verify" ] && [ "$MODE" != "relaxed" ]; then
    echo "Error: Unknown mode: $MODE"
    usage
    exit 1
fi

run_project() {
    local project_dir="$1"
    local project_name="$(basename "$project_dir")"

    echo "========================================"
    echo "Project: $project_name"
    echo "========================================"

    # Find .swproj file
    local swproj=$(find "$project_dir" -maxdepth 1 -name "*.swproj" | head -1)
    if [ -z "$swproj" ]; then
        echo "Warning: No .swproj file found in $project_dir, skipping"
        return 0
    fi

    local swproj_name="$(basename "$swproj")"
    echo "Using project file: $swproj_name"

    cd "$project_dir"
    rm -rf deepssm groomed *_particles

    shapeworks deepssm --name "$swproj_name" --all

    # Verify results
    local baseline_file="exact_check_${project_name}.txt"
    local verify_args=""

    if [ "$MODE" = "save" ]; then
        verify_args="--exact_check save --baseline_file $baseline_file"
    elif [ "$MODE" = "verify" ]; then
        verify_args="--exact_check verify --baseline_file $baseline_file"
    else
        verify_args="--expected 10 --tolerance 1.0"
    fi

    python "${SCRIPT_DIR}/verify_deepssm_results.py" . $verify_args

    echo ""
}

echo "Extended DeepSSM Tests"
echo "Base directory: ${BASE_DIR}"
echo "Mode: ${MODE}"
echo ""

# Find all project directories (directories containing .swproj files)
ran_any=false
for project_dir in "$BASE_DIR"/*/; do
    if [ ! -d "$project_dir" ]; then
        continue
    fi

    project_name="$(basename "$project_dir")"

    # Skip if specific project requested and this isn't it
    if [ "$PROJECT" != "all" ] && [ "$PROJECT" != "$project_name" ]; then
        continue
    fi

    # Check if this directory has a .swproj file
    if ls "$project_dir"/*.swproj 1>/dev/null 2>&1; then
        run_project "$project_dir"
        ran_any=true
    fi
done

if [ "$ran_any" = false ]; then
    if [ "$PROJECT" = "all" ]; then
        echo "Error: No projects found in $BASE_DIR"
        echo "Each project should be a subdirectory containing a .swproj file."
    else
        echo "Error: Project not found: $PROJECT"
    fi
    exit 1
fi

echo "========================================"
echo "All projects complete!"
echo "========================================"
