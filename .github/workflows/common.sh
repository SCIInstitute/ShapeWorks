
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
    if [ -f ~/.bashrc ] ; then
	source ~/.bashrc
    fi
    PLATFORM="linux"
    CONDA_PATH="/opt/conda/envs/shapeworks"
    DEP_PATH="$HOME/install"
    CCACHE_DIR="$HOME/.ccache"
    USE_CCACHE="ON"
    SUFFIX="tar.gz"
else
    source /c/Users/runneradmin/.bash_profile
    PLATFORM="windows"
    CONDA_PATH="C:\Miniconda3\envs\shapeworks"
    DEP_PATH="C:\deps"
    USE_CCACHE="OFF"
    SUFFIX="7z"
fi

compress_file() {
    if [[ "$PLATFORM" == "windows" ]]; then
	7z -spf a "$1" "$2"
    else
	tar --use-compress-program=pigz -cf "$1" "$2"
    fi
}

decompress_file() {
    if [[ "$PLATFORM" == "windows" ]]; then
	if [ 7z t $1 ]; then
	    7z -spf x $1
	fi
    else
	if [ tar -tzf $2 >/dev/null ]; then
	    tar --use-compress-program=pigz -xf "$1"
	fi
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
DEP_HASH="${CONDA_HASH}_${DEP_HASH}"
echo "DEP_HASH = ${DEP_HASH}"
DEP_FILE="dep-${PLATFORM}-${DEP_HASH}-${BUILD_TYPE}.${SUFFIX}"

CCACHE_FILE="${PLATFORM}-ccache.${SUFFIX}"
