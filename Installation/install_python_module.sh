#!/bin/bash -x

# Installs shapeworks Python module
# Ensures conda Python module is able to locate correct shapeworks_py binary module.


CONDA_INSTALL_DIR=`pip show shapeworks | grep Location | awk '{print $2}'`/shapeworks

# check if $CONDA_INSTALL_DIR is under $CONDA_PREFIX
if [[ "$CONDA_INSTALL_DIR" != "$CONDA_PREFIX"* ]]; then
    echo "Not running replace_strings.py because ${CONDA_INSTALL_DIR} is not under ${CONDA_PREFIX}"
    exit 1
fi

python ./Installation/replace_strings.py $CONDA_INSTALL_DIR/setup_path.py placeholder_string `pwd`/bin
