#!/bin/bash -x

echo "#############################"
echo "# Restore Caches            #"
echo "#############################"

PLATFORM=linux

conda_hash=`sha1sum install_shapeworks.sh | awk '{ print $1 }'`
echo "conda_hash = ${conda_hash}"
file="conda-${PLATFORM}-${conda_hash}.tar.gz"

cd /
scp runner@${CACHE_HOST}:github/${PLATFORM}-ccache.tar.gz .
if [ -f ${PLATFORM}-ccache.tar.gz ] ; then
    tar --use-compress-program=pigz -xf ${PLATFORM}-ccache.tar.gz
fi

cd /
scp runner@${CACHE_HOST}:github/${file} .


if [ -f ${file} ] ; then
    echo "Conda Cache file was found"
    tar --use-compress-program=pigz -xf $file
fi


