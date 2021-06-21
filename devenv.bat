rem
rem Configures developer environment such that:
rem - PATH and PYTHONPATH point to $BUILD/bin
rem - PYTHONPATH points to each module in $SOURCE/Python
rem
rem Example when build directory is inside source:
rem   (shapeworks) ~/code/ShapeWorks$ source ./devenv.sh `pwd` `pwd`/build_debug
rem
rem After sourcing this, processes in the environment will use executables from
rem the given build and import modules from the developer's source (including the
rem compiled portion of the Python bindings).
rem

set argC=0
for %%x in (%*) do Set /A argC+=1
echo %argC%

if not %argC%==2 (
  echo Must call using "devenv SOURCE_DIR BUILD_DIR"
  goto :eof
)

set SOURCE=%~1
set BUILD=%~2
echo source: %SOURCE%
echo build: %BUILD%

set PATH=%BUILD%\bin;%PATH%
set PYTHONPATH=%BUILD%\bin;%PYTHONPATH%

rem add each module in ${SOURCE}/Python to the PYTHONPATH
for /d %%D in ("%SOURCE%\Python\*") DO for /d %%M in ("%%D\*") DO call set PYTHONPATH=%%M;%%PYTHONPATH%%
