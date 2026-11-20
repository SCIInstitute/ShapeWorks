
if [[ "$OSTYPE" == "darwin"* ]]; then
    PATH="$(brew --prefix)/opt/coreutils/libexec/gnubin:$PATH"
fi


if [[ "$OSTYPE" == "darwin"* ]]; then
    source ~/.zshrc
    # check arch
    if [[ "$(uname -m)" == "arm64" ]]; then
        PLATFORM="mac-arm64"
	CONDA_PATH="/Users/runner/miniconda3/envs/shapeworks"
    else
        PLATFORM="mac-intel"
	CONDA_PATH="/usr/local/miniconda/envs/shapeworks"
    fi
    DEP_PATH="$HOME/install"
    CCACHE_DIR="/Users/runner/Library/Caches/ccache"
    USE_CCACHE="ON"
    PATH="$(brew --prefix)/opt/gnu-tar/libexec/gnubin:$PATH"
elif [[ "$OSTYPE" == "linux"* ]]; then
    if [ -f ~/.bashrc ] ; then
	source ~/.bashrc
    fi
    PLATFORM="linux"
    CONDA_PATH="/opt/conda/envs/shapeworks"
    DEP_PATH="$HOME/install"
    CCACHE_DIR="$HOME/.ccache"
    USE_CCACHE="ON"
else
    source /c/Users/runneradmin/.bash_profile
    PLATFORM="windows"
    CONDA_PATH="C:\Miniconda\envs\shapeworks"
    DEP_PATH="C:\deps"
    USE_CCACHE="OFF"
fi

if [[ "$USE_CCACHE" == "ON" ]]; then
    mkdir -p $CCACHE_DIR
    echo "compression = true" > $CCACHE_DIR/ccache.conf
fi
