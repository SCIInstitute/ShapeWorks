#!/bin/bash -x

echo "#############################"
echo "# Copy Artifact             #"
echo "#############################"

. $GITHUB_WORKSPACE/.github/workflows/common.sh

TARGET="/tmp/${1}.${SUFFIX}"
compress_file $TARGET "${2}"


if [[ "$PLATFORM" == "windows" ]]; then
    choco install rsync -y
    TARGET=`cygpath -m {TARGET}`
    echo "Using target ${TARGET}"
fi

# try 5 times
for run in {1..5}; do
    rsync ${TARGET} runner@${CACHE_HOST}:github/artifacts
    sleep 1
done
rm ${TARGET}
