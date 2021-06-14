REM Installs the shapeworks python module.
REM Usage: call .\install_python_module.bat <path to directory with shapeworks_py pyd and its dll dependencies>
REM  NOTE: assumes all library dependencies are in this directory

REM install pip module
call pip install Python/shapeworks

REM copy compiled portion of package to module directory
call copy %1\shapeworks_py*.pyd %CONDA_PREFIX%\lib\site-packages\shapeworks

REM copy dependencies of the .pyd to the module directory (it doesn't need quite all 182MB of these, but most all)
call copy %1\*.dll %CONDA_PREFIX%\lib\site-packages\shapeworks
