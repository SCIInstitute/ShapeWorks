#!/bin/bash -x

echo "#############################"
echo "# Copy Artifact             #"
echo "#############################"

. $GITHUB_WORKSPACE/.github/workflows/common.sh

TARGET="/tmp/${1}.${SUFFIX}"
compress_file $TARGET "${@:2}"

scp ${TARGET} runner@${CACHE_HOST}:github/artifacts
rm ${TARGET}
