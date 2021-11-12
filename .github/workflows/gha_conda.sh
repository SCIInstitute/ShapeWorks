#!/bin/bash -x

echo "#############################"
echo "# Install conda packages    #"
echo "#############################"

PLATFORM=linux

conda_hash=`sha1sum install_shapeworks.sh | awk '{ print $1 }'`
echo "conda_hash = ${conda_hash}"
file="conda-${PLATFORM}-${conda_hash}.tar.gz"

if [ -d /opt/conda/envs/shapeworks ] ; then
    echo "Using cached conda installs"
else
    echo "Conda Cache file was not found"
    cd $GITHUB_WORKSPACE
    ls
    source ./install_shapeworks.sh

    echo "Create and store cache"
    tar --use-compress-program=pigz -cf ${file} /opt/conda/envs/shapeworks
    scp ${file} runner@${CACHE_HOST}:github
    rm ${file}
fi
