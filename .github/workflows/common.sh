
if [[ "$OSTYPE" == "darwin"* ]]; then
    source ~/.zshrc
    PLATFORM="mac"
    CONDA_PATH="/usr/local/miniconda/envs/shapeworks"
    DEP_PATH="$HOME/install"
    CCACHE_DIR="/Users/runner/Library/Caches/ccache"
    USE_CCACHE="ON"
    PATH="$(brew --prefix)/opt/gnu-tar/libexec/gnubin:$PATH"
    SUFFIX="tar.gz"
elif [[ "$OSTYPE" == "linux"* ]]; then
    source ~/.bashrc
    PLATFORM="linux"
    CONDA_PATH="/opt/conda/envs/shapeworks"
    DEP_PATH="$HOME/install"
    CCACHE_DIR="$HOME/.ccache"
    USE_CCACHE="ON"
    SUFFIX="tar.gz"
else
    source ~/.bashrc
    PLATFORM="windows"
    CONDA_PATH="C:\Miniconda3\envs\shapeworks"
    DEP_PATH="C:\deps"
    USE_CCACHE="OFF"
    SUFFIX="7z"
fi

compress_file() {
    if [[ "$OSTYPE" == "windows"* ]]; then
	7z -spf a "$1" "$2"
    else
	tar --use-compress-program=pigz -cf "$1" "$2"
    fi
}

decompress_file() {
    if [[ "$PLATFORM" == "windows"* ]]; then
	7z -spf x $1
    else
	tar --use-compress-program=pigz -xf "$1"
    fi
}


if [[ "$USE_CCACHE" == "ON" ]]; then
    mkdir -p $CCACHE_DIR
    echo "compression = true" > $CCACHE_DIR/ccache.conf
fi

CONDA_HASH=`sha1sum install_shapeworks.sh | awk '{ print $1 }'`
echo "CONDA_HASH = ${CONDA_HASH}"
CONDA_FILE="conda-${PLATFORM}-${CONDA_HASH}.${SUFFIX}"

DEP_HASH=`sha1sum build_dependencies.sh | awk '{ print $1 }'`
echo "DEP_HASH = ${DEP_HASH}"
DEP_FILE="dep-${PLATFORM}-${DEP_HASH}.${SUFFIX}"

CCACHE_FILE="${PLATFORM}-ccache.${SUFFIX}"
