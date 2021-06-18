#!/bin/bash

# Restore original PATH on conda deactivate.
if [[ -z "$_SHAPEWORKS_BIN_PATH" ]]; then
    export PATH=`echo $PATH | sed "s*$_SHAPEWORKS_BIN_PATH:**g"`
fi
