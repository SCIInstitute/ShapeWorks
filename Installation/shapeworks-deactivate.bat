@echo off
setlocal EnableDelayedExpansion

REM Restore original PATH on conda deactivate.
if defined _SHAPEWORKS_BIN_PATH (
    set PATH=!PATH:%_SHAPEWORKS_BIN_PATH%;=!
)
