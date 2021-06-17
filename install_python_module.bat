REM Installs the shapeworks python module.
REM  NOTE: assumes all library dependencies are in this directory\bin

REM install pip module
call pip install Python\shapeworks

set CONDA_INSTALL_DIR=%CONDA_PREFIX%\lib\site-packages\shapeworks

call python .\update_installed_python_module_binary_path.py "%CONDA_INSTALL_DIR%\setup_path.py" "%cd%\bin"
