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
    conda clean -p -t

    echo "Create and store cache"
    cd /
    compress_file /tmp/${CONDA_FILE} "${CONDA_PATH}"
    scp /tmp/${CONDA_FILE} runner@${CACHE_HOST}:github
    rm /tmp/${CONDA_FILE}
fi

conda init bash
