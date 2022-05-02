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
    export SDKROOT=$HOME/MacOSX10.13.sdk # only needed for MacOS obviously
    ./build_dependencies.sh --build-type=$BUILD_TYPE
    rm -rf $BUILD_DIR

    echo "Create and store cache"
    cd /
    compress_file ${FILE} "${DEP_PATH}"
    scp ${FILE} runner@${CACHE_HOST}:github
    rm ${FILE}
fi
