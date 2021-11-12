#!/bin/bash -x

echo "#############################"
echo "# Install conda packages    #"
echo "#############################"

. ${GITHUB_WORKSPACE}/.github/workflows/common.sh

if [ -d ${CONDA_PATH} ] ; then
    echo "Using cached conda installs"
else
    echo "Conda Cache file was not found"
    cd ${GITHUB_WORKSPACE}

    # run install
    source ./install_shapeworks.sh

    echo "Create and store cache"
    tar --use-compress-program=pigz -cf ${CONDA_FILE} ${CONDA_PATH}
    scp ${CONDA_FILE} runner@${CACHE_HOST}:github
    rm ${CONDA_FILE}
fi

conda init bash
