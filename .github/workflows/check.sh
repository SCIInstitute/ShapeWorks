#!/bin/bash

env
hash=($(hash install_shapeworks.sh))
file=conda-linux-${hash}.tar.gz

cd /
scp runner@${CACHE_HOST}:github/${file} .
if [ -f ${file} ] ; then
    echo "Cache file was found"
    tar --use-compress-program=pigz -xf $file
else
    echo "Cache file was not found"
    cd $RUNNER_WORKSPACE
    ls
    source ./install_shapeworks.sh
fi


