PLATFORM=linux

source ~/.bashrc

CONDA_HASH=`sha1sum install_shapeworks.sh | awk '{ print $1 }'`
echo "CONDA_HASH = ${CONDA_HASH}"
CONDA_FILE="conda-${PLATFORM}-${CONDA_HASH}.tar.gz"
CONDA_PATH="/opt/conda/envs/shapeworks"

DEP_HASH=`sha1sum build_dependencies.sh | awk '{ print $1 }'`
echo "dep_hash = ${dep_hash}"
DEP_FILE="dep-${PLATFORM}-${DEP_HASH}.tar.gz"
DEP_PATH="$HOME/install"

CCACHE_FILE="${PLATFORM}-ccache.tar.gz"
