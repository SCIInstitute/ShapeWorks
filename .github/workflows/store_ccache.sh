#!/bin/bash -x

echo "#############################"
echo "# Store CCache              #"
echo "#############################"

. $GITHUB_WORKSPACE/.github/workflows/common.sh

which ccache
ccache -p
ccache -s
ccache -s
cd /
tar --use-compress-program=pigz -cf ${CCACHE_FILE} $HOME/.ccache

scp ${CCACHE_FILE} runner@${CACHE_HOST}:github
rm ${CCACHE_FILE}
