@echo off

REM Store _SHAPEWORKS_BIN_PATH to be removed on conda deactivate.
set _SHAPEWORKS_BIN_PATH=shapeworks_placeholder_string

REM Add _SHAPEWORKS_BIN_PATH to PATH on conda activate.
set PATH=%_SHAPEWORKS_BIN_PATH%;%PATH%
