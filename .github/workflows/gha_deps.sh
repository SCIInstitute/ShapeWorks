#!/bin/bash -x

echo "#############################"
echo "# Build dependencies        #"
echo "#############################"

. ${GITHUB_WORKSPACE}/.github/workflows/common.sh

if [ -d ${DEP_PATH} ] ; then
    echo "Using cached dependencies"
else
    echo "Dep file was not found"
    cd ${GITHUB_WORKSPACE}

    # build dependencies
    conda activate shapeworks
    export INSTALL_DIR="$DEP_PATH"
    export BUILD_DIR=$HOME/build
    export FILE="/tmp/${DEP_FILE}"
    if [[ "$PLATFORM" == "windows" ]]; then
      export INSTALL_DIR="C:\deps"
      export BUILD_DIR="/c/bdeps"
      export FILE="${DEP_FILE}"
    fi

    NPROCS=4

    if [[ "$PLATFORM" == "linux" ]]; then
        # GHA runner is running out of resources with 4 now on linux
	NPROCS=2
    fi

    ./build_dependencies.sh --build-type=$BUILD_TYPE --num-procs=$NPROCS --clean-after
    rm -rf $BUILD_DIR
fi
