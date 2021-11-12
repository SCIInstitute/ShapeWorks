#!/bin/bash

echo "#############################"
echo "pwd = `pwd`"
env
echo "#############################"


ls ShapeWorks

hash=($(hash install_shapeworks.sh))
echo "hash = ${hash}"
file=conda-linux-${hash}.tar.gz

cd /
scp runner@${CACHE_HOST}:github/${file} .
if [ -f ${file} ] ; then
    echo "Cache file was found"
    tar --use-compress-program=pigz -xf $file
else
    echo "Cache file was not found"
    cd $GITHUB_WORKSPACE
    ls
    source ./install_shapeworks.sh
fi


