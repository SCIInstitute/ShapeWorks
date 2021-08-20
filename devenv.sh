#
# Configures developer environment such that:
# - PATH and PYTHONPATH point to $BUILD_BIN
# - PYTHONPATH points to each module in $SOURCE/Python
#
# Example when build directory is inside source:
#   (shapeworks) ~/code/ShapeWorks$ source ./devenv.sh ./build/bin
#
# After sourcing this, processes in the environment will use executables from
# the given build and import modules from the developer's source (including the
# compiled portion of the Python bindings).
#

(return 0 2>/dev/null) && sourced=1 || sourced=0

if [[ "$sourced" == "0" ]]; then
    echo "ERROR: must call this script using \"source ./devenv.sh BUILD_BIN\""
    exit 1
fi

if [[ "$#" -ne 1 ]]; then
    echo "ERROR: must call this script using \"source ./devenv.sh BUILD_BIN\""
    return 1
fi    

SOURCE="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
echo "Source directory: ${SOURCE}"
BUILD="$( cd "$1" &> /dev/null && pwd )"
echo "Binary directory: ${BUILD}"

export PATH=${BUILD}:$PATH
export PYTHONPATH=${BUILD}:$PYTHONPATH

# add each module in ${SOURCE}/Python to the PYTHONPATH
for M in ${SOURCE}/Python/*/; do
    export PYTHONPATH=${M}:$PYTHONPATH
done

# Set the python path for studio
mkdir $HOME/.shapeworks ; python -c "import sys; print('\n'.join(sys.path))" > $HOME/.shapeworks/python_path.txt
