#!/bin/bash -x

echo "#############################"
echo "# Restore Caches            #"
echo "#############################"

. $GITHUB_WORKSPACE/.github/workflows/common.sh

# based on root folder
cd /

if [[ "$USE_CCACHE" == "ON" ]]; then
    # Restore ccache
    scp runner@${CACHE_HOST}:github/${CCACHE_FILE} .
    if [ -f ${CCACHE_FILE} ] ; then
	echo "ccache file was found"
	tar --use-compress-program=pigz -xf ${CCACHE_FILE}
	rm $CCACHE_FILE
    fi
fi

# Restore conda installs
scp runner@${CACHE_HOST}:github/${CONDA_FILE} .
if [ -f ${CONDA_FILE} ] ; then
    echo "Conda Cache file was found"
    tar --use-compress-program=pigz -xf $CONDA_FILE
    rm $CONDA_FILE
fi

# Restore dependencies
scp runner@${CACHE_HOST}:github/${DEP_FILE} .
if [ -f ${DEP_FILE} ] ; then
    echo "Dependency Cache file was found"
    tar --use-compress-program=pigz -xf $DEP_FILE
    rm $DEP_FILE
fi

which ccache
ccache -p
ccache -s
