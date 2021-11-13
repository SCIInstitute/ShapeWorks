#!/bin/bash -x

echo "#############################"
echo "# Store CCache              #"
echo "#############################"

PLATFORM=linux

FILE="${PLATFORM}-ccache.tar.gz"

ccache -s
cd /
tar --use-compress-program=pigz -cf ${FILE} /root/.ccache

scp ${FILE} runner@${CACHE_HOST}:github
rm ${FILE}
