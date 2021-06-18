#!/bin/bash

# Store _SHAPEWORKS_BIN_PATH to be removed on conda deactivate.
export _SHAPEWORKS_BIN_PATH=shapeworks_placeholder_string

# Add _SHAPEWORKS_BIN_PATH to PATH on conda activate.
export PATH=$_SHAPEWORKS_BIN_PATH:$PATH
