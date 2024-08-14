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
    source ./install_shapeworks.sh --developer
    conda clean -p -t -y
    pip cache info
    pip cache purge
fi

conda init bash
