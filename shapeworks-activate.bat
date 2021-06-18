@echo off

REM Adds shapeworks bin directory to PATH on conda activate.
REM Stores existing PATH to be restored on conda deactivate.

set _CONDA_SHAPEWORKS_PREV_PATH=%PATH%

set PATH="placeholder_string";$PATH
