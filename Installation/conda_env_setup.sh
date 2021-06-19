#!/bin/bash -x

# Copy the [de]activate scripts to $CONDA_PREFIX/etc/conda/[de]activate.d.
# This will allow them to be run on environment activation.
for CHANGE in "activate" "deactivate"
do
    mkdir -p "${CONDA_PREFIX}/etc/conda/${CHANGE}.d"
    cp "./Installation/shapeworks-${CHANGE}.sh" "${CONDA_PREFIX}/etc/conda/${CHANGE}.d/shapeworks-${CHANGE}.sh"
    python ./Installation/replace_strings.py "${CONDA_PREFIX}/etc/conda/${CHANGE}.d/shapeworks-${CHANGE}.sh" shapeworks_placeholder_string "\"`pwd`/bin\""
done
