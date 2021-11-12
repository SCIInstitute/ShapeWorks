#!/bin/bash

env
hash=($(hash install_shapeworks.sh))
file=conda-linux-${hash}.tar.gz

cd /
scp runner@${CACHE_HOST}:github/${file} .
if [ -f ${file} ] ; then
    tar --use-compress-program=pigz -xf $file
else
    cd $RUNNER_WORKSPACE
    source ./install_shapeworks.sh
fi


