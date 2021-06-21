#
# Configures developer environment such that:
# - PATH and PYTHONPATH point to $BUILD/bin
# - PYTHONPATH points to each module in $SOURCE/Python
#
# Example when build directory is inside source:
#   (shapeworks) ~/code/ShapeWorks$ source ./devenv.sh `pwd` `pwd`/build_debug
#
# After sourcing this, processes in the environment will use executables from
# the given build and import modules from the developer's source (including the
# compiled portion of the Python bindings).
#

(return 0 2>/dev/null) && sourced=1 || sourced=0

if [[ "$sourced" == "0" ]]; then
    echo "ERROR: must call this script using \"source ./devenv.sh <source_dir> <build_dir>\""
    exit 1
fi

if [[ "$#" -ne 2 ]]; then
    echo "ERROR: must pass both SOURCE_DIR and BUILD_DIR"
    exit 1
fi    

SOURCE=$1
BUILD=$2

export PATH=${BUILD}/bin:$PATH
export PYTHONPATH=${BUILD}/bin:$PYTHONPATH

# add each module in ${SOURCE}/Python to the PYTHONPATH
for M in ${SOURCE}/Python/*/; do
    export PYTHONPATH=${M}:$PYTHONPATH
done
