#!/bin/bash -x

echo "#############################"
echo "# Download Test Data        #"
echo "#############################"

set +e

BASE=`pwd`

. $GITHUB_WORKSPACE/.github/workflows/common.sh

# based on root folder
cd /

WGET="wget"

# if windows
if [ -f /usr/bin/wget ]; then
    WGET="C:\msys64\usr\bin\wget.exe"
fi


# Tiny test data downloads
for run in {1..5}; do
    if [ ! -f /tmp/tiny_test_cache.tar.gz ] ; then
	cd /tmp
	$WGET https://www.sci.utah.edu/~shapeworks/doc-resources/zips/tiny_test_cache.tar.gz || true
	sleep 1
    fi
done

# Decompress if successful
if [ -f /tmp/tiny_test_cache.tar.gz ] ; then
    cd $BASE/Examples/Python
    tar -xzvf /tmp/tiny_test_cache.tar.gz
    rm /tmp/tiny_test_cache.tar.gz
    echo "Decompressed tiny test data"
else
    echo "Failed to download tiny test data"
fi

