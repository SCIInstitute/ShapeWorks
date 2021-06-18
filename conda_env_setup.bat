setlocal EnableDelayedExpansion

:: Copy the [de]activate scripts to %PREFIX%\etc\conda\[de]activate.d.
:: This will allow them to be run on environment activation.
for %%CHANGE in (activate deactivate) DO (
    if not exist "%CONDA_PREFIX%\etc\conda\%%CHANGE.d" mkdir "%CONDA_PREFIX%\etc\conda\%%CHANGE.d"
    copy shapeworks-%%CHANGE.bat "%CONDA_PREFIX%\etc\conda\%%CHANGE.d\shapeworks-%%CHANGE.bat"
    call python .\replace_strings.py "%CONDA_PREFIX%\etc\conda\%%CHANGE.d\shapeworks-%%CHANGE.bat" placeholder_string "%cd%\bin"
    :: Copy unix shell activation scripts, needed by Windows Bash users
    copy shapeworks-%%CHANGE.sh "%CONDA_PREFIX%\etc\conda\%%CHANGE.d\shapeworks-%%CHANGE.sh"
    call python .\replace_strings.py "%CONDA_PREFIX%\etc\conda\%%CHANGE.d\shapeworks-%%CHANGE.sh" placeholder_string "%cd%\bin"
)
