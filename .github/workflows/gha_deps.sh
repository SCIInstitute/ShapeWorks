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

    if [[ "$PLATFORM" == "linux" ]]; then
	export VTK_EXTRA_OPTIONS="-DOPENGL_EGL_INCLUDE_DIR:PATH=/usr/include -DOPENGL_opengl_LIBRARY=/usr/lib/x86_64-linux-gnu/libGL.so -DOPENGL_glx_LIBRARY=/usr/lib/x86_64-linux-gnu/libGL.so"
    fi

    export SDKROOT=$HOME/MacOSX10.13.sdk # only needed for MacOS obviously
    ./build_dependencies.sh --build-type=$BUILD_TYPE --num-procs=3
    rm -rf $BUILD_DIR

    echo "Create and store cache"
    cd /
    compress_file ${FILE} "${DEP_PATH}"
    scp ${FILE} runner@${CACHE_HOST}:github
    rm ${FILE}
fi
