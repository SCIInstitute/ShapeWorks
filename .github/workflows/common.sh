
if [[ "$OSTYPE" == "darwin"* ]]; then
    source ~/.zshrc
    PLATFORM="mac"
    CONDA_PATH="/usr/local/miniconda/envs/shapeworks"
    DEP_PATH="$HOME/install"
    CCACHE_DIR="/Users/runner/Library/Caches/ccache"
    USE_CCACHE="ON"
elif [[ "$OSTYPE" == "linux"* ]]; then
    source ~/.bashrc
    PLATFORM="linux"
    CONDA_PATH="/opt/conda/envs/shapeworks"
    DEP_PATH="$HOME/install"
    CCACHE_DIR="$HOME/.ccache"
    USE_CCACHE="ON"
else
    source ~/.bashrc
    PLATFORM="windows"
    CONDA_PATH="?"
    DEP_PATH="?"
    USE_CCACHE="OFF"
fi

if [[ "$USE_CCACHE" == "ON" ]]; then
    echo "compression = true" > $CCACHE_DIR/ccache.conf
fi

CONDA_HASH=`sha1sum install_shapeworks.sh | awk '{ print $1 }'`
echo "CONDA_HASH = ${CONDA_HASH}"
CONDA_FILE="conda-${PLATFORM}-${CONDA_HASH}.tar.gz"

DEP_HASH=`sha1sum build_dependencies.sh | awk '{ print $1 }'`
echo "DEP_HASH = ${DEP_HASH}"
DEP_FILE="dep-${PLATFORM}-${DEP_HASH}.tar.gz"

CCACHE_FILE="${PLATFORM}-ccache.tar.gz"
