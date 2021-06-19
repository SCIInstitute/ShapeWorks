setlocal EnableDelayedExpansion

:: format windows path into posix path
set BIN_PATH=%cd%\bin
set BIN_PATH=%BIN_PATH:\=/%
set BIN_PATH=/%BIN_PATH::=%

:: Copy the [de]activate scripts to %PREFIX%\etc\conda\[de]activate.d.
:: This will allow them to be run on environment activation.
for %%F in (activate deactivate) DO (
    if not exist "%CONDA_PREFIX%\etc\conda\%%F.d" mkdir "%CONDA_PREFIX%\etc\conda\%%F.d"
    copy .\Installation\shapeworks-%%F.bat "%CONDA_PREFIX%\etc\conda\%%F.d\shapeworks-%%F.bat"
    call python .\Installation\replace_strings.py "%CONDA_PREFIX%\etc\conda\%%F.d\shapeworks-%%F.bat" shapeworks_placeholder_string "%cd%\bin"

    :: Copy unix shell activation scripts, needed by Windows Bash users
    copy .\Installation\shapeworks-%%F.sh "%CONDA_PREFIX%\etc\conda\%%F.d\shapeworks-%%F.sh"
    call python .\Installation\replace_strings.py "%CONDA_PREFIX%\etc\conda\%%F.d\shapeworks-%%F.sh" shapeworks_placeholder_string "\"%BIN_PATH%\""
)
