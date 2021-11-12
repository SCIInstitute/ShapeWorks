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
    INSTALL_DIR=$HOME/install
    BUILD_DIR=$HOME/build
    ./build_dependencies.sh --build-type=$BUILD_TYPE
    rm -rf $HOME/build

    echo "Create and store cache"
    tar --use-compress-program=pigz -cf ${DEP_FILE} ${DEP_PATH}
    scp ${DEP_FILE} runner@${CACHE_HOST}:github
    rm ${DEP_FILE}
fi
