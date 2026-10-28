#!/bin/bash -x

echo "#############################"
echo "# Download Test Data        #"
echo "#############################"

set +e

BASE=`pwd`

. $GITHUB_WORKSPACE/.github/workflows/common.sh

# based on root folder
cd /

# if platform is windows, install wget
if [ "$PLATFORM" == "windows" ]; then
    choco install wget --no-progress
fi

# Tiny test data. A failed attempt leaves a partial file behind, so retry on whether the
# archive is complete rather than on whether it exists, and let wget resume it.
cd /tmp
for run in {1..5}; do
    if tar -tzf /tmp/tiny_test_cache.tar.gz > /dev/null 2>&1 ; then
	break
    fi
    wget --continue --tries=3 --timeout=60 https://www.sci.utah.edu/~shapeworks/doc-resources/zips/tiny_test_cache.tar.gz || true
    sleep 1
done

# Only extract a complete archive: a half-extracted cache sends the use cases to the web.
if tar -tzf /tmp/tiny_test_cache.tar.gz > /dev/null 2>&1 ; then
    cd $BASE/Examples/Python
    tar -xzvf /tmp/tiny_test_cache.tar.gz
    rm /tmp/tiny_test_cache.tar.gz
    echo "Decompressed tiny test data"
else
    echo "Failed to download tiny test data"
fi

