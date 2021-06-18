#!/bin/bash

# Adds shapeworks bin directory to PATH on conda activate.
# Stores existing PATH to be restored on conda deactivate.

export _CONDA_SHAPEWORKS_PREV_PATH=$PATH

export PATH="placeholder_string":$PATH
