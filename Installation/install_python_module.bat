REM Installs the shapeworks python module.
REM  NOTE: assumes all library dependencies are in this directory\bin

REM install pip module
call pip install Python\shapeworks

set CONDA_INSTALL_DIR=%CONDA_PREFIX%\lib\site-packages\shapeworks

:: format windows path to use forward slashes
set BIN_PATH=%cd%\bin
set BIN_PATH=%BIN_PATH:\=/%

call python .\Installation\replace_strings.py "%CONDA_INSTALL_DIR%\setup_path.py" placeholder_string %BIN_PATH%
