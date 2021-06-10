#!/bin/bash -x

if [ "$#" -lt 1 ]; then
    echo "Installs python module binary (shapeworks_py) into conda, optionally adding necessary runpaths to the library."
    echo "Usage: $0 <shapeworks_py_library> [<lib_dir_1> ... <lib_dir_n>]"
    exit 1
fi

# install pip module
if ! pip install Python/shapeworks; then exit -1; fi

PYTHON_LIBRARY=$1; shift
CONDA_INSTALL_DIR=`pip show shapeworks | grep Location | awk '{print $2}'`/shapeworks

# install shapeworks_py, compiled portion of package
if [[ "$(uname)" == "Linux" ]]; then

    # if unspecified, use package path from github action (see package.sh)
    if [ "$#" -lt 1 ]; then
        if [[ "$PR_NUMBER" != "" ]]; then
            VERSION="ShapeWorks-PR-${PR_NUMBER}-linux"
        else
            VERSION="ShapeWorks-$(git describe --tags)-linux"
        fi
	      LIBDIR="`pwd`/package/${VERSION}/lib"

        # Special case for when we are on the master branch (dev releases)
        BRANCH=$(git rev-parse --abbrev-ref HEAD)
        if [[ "$BRANCH" == "master" ]]; then
            NUMBER=$(git rev-list start_dev_releases..HEAD --count)
            HASH=$(git rev-parse --short HEAD)
            VERSION="ShapeWorks-dev-${NUMBER}-${HASH}-linux"
        fi
    elif [ "$#" -gt 1 ]; then
        echo "error: linux shared libraries only have one RUNPATH, but more than one requested"
        exit -1
    else
        LIBDIR=$1
    fi

    # add libdir to runpath of python library
    echo "adding ${LIBDIR}..."
    patchelf --set-rpath "${LIBDIR}" "${PYTHON_LIBRARY}"

elif [[ "$(uname)" == "Darwin" ]]; then

    # add each libdir to runpath of python library
    for dir in "$@"
    do
        echo "adding $dir..."
        install_name_tool -add_rpath "${dir}" "${PYTHON_LIBRARY}"
    done

else
    echo "Unknown operating system: ${uname}"
    exit 1
fi

# move shapeworks_py library to conda install dir
mv "${PYTHON_LIBRARY}" "${CONDA_INSTALL_DIR}"
