#!/bin/bash -x

echo "#############################"
echo "# Store CCache              #"
echo "#############################"

. $GITHUB_WORKSPACE/.github/workflows/common.sh

if [[ "$USE_CCACHE" == "ON" ]]; then
    which ccache
    ccache -p
    ccache -s
    cd /
    compress_file "/tmp/${CCACHE_FILE}" "${CCACHE_DIR}"

    scp /tmp/${CCACHE_FILE} runner@${CACHE_HOST}:github
    rm /tmp/${CCACHE_FILE}
fi
