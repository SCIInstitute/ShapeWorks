rem
rem Configures developer environment such that:
rem - PATH and PYTHONPATH point to $BUILD_BIN
rem - PYTHONPATH points to each module in $SOURCE/Python
rem
rem Example when build directory is inside source:
rem   (shapeworks) c:\code\ShapeWorks>devenv .\build\bin\Debug
rem
rem After calling this, processes in the environment will use executables from
rem the given build and import modules from the developer's source (including the
rem compiled portion of the Python bindings).
rem

set argC=0
for %%x in (%*) do Set /A argC+=1
echo %argC%

if not %argC%==1 (
  echo Must call using "devenv BUILD_BIN"
  goto :eof
)

set SOURCE=%~dp0

rem batch-relative trick to get full path to BUILD_BIN
pushd .
cd %~dp0
set BUILD=%~f1
popd

echo source: %SOURCE%
echo build: %BUILD%

set PATH=%BUILD%;%PATH%
set PYTHONPATH=%BUILD%;%PYTHONPATH%

rem add each module in ${SOURCE}/Python to the PYTHONPATH
for /d %%D in ("%SOURCE%\Python\*") DO for /d %%M in ("%%D\*") DO call set PYTHONPATH=%%M;%%PYTHONPATH%%

md %USERPROFILE%\.shapeworks
python -c "import sys; print('\n'.join(sys.path))" > %USERPROFILE%\.shapeworks/python_path.txt
