REM remove existing shapeworks env if any
set CONDAENV=shapeworks
IF NOT "%1"=="" SET CONDAENV=%1

REM === Check for Smart App Control (Windows 11) ===
REM Smart App Control will block unsigned DLLs (VTK, PyTorch, etc.) that ShapeWorks
REM installs via pip, causing import failures. Warn the user before proceeding.
set SAC_STATE=
for /f "delims=" %%s in ('powershell -NoProfile -Command "(Get-MpComputerStatus).SmartAppControlState" 2^>nul') do set SAC_STATE=%%s
if /i "%SAC_STATE%"=="On" (
    echo.
    echo ============================================================
    echo  WARNING: Smart App Control is ENABLED on this computer.
    echo ============================================================
    echo.
    echo  ShapeWorks Studio itself will still run, but the Python
    echo  features ^(use cases, notebooks, DeepSSM^) will fail because
    echo  Smart App Control blocks the unsigned Python libraries
    echo  ^(VTK, PyTorch, etc.^) with errors like:
    echo.
    echo    "An Application Control policy has blocked this file"
    echo.
    echo  To use the Python features, turn Smart App Control OFF:
    echo.
    echo    1. Click the Start button and type "Smart App Control"
    echo    2. Click "Smart App Control settings"
    echo    3. Select "Off"
    echo.
    echo  IMPORTANT: Turning Smart App Control off is permanent.
    echo  To turn it back on, you must reset Windows.
    echo.
    echo  See https://sciinstitute.github.io/ShapeWorks/users/install.html
    echo  for more information.
    echo.
    echo  Press Ctrl+C to cancel, or any key to continue installing
    echo  ^(install will succeed, but Python features won't work
    echo  until SAC is off^).
    echo ============================================================
    echo.
    pause
)

echo "creating new conda environment for ShapeWorks called %CONDAENV%..."

REM Accept Anaconda TOS for required channels (must happen before any conda command that touches default channels)
call conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/main
call conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/r
call conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/msys2

REM remove any existing environment with this name
call conda env remove -n %CONDAENV% -y

REM update anaconda
call conda config --add channels anaconda
call conda config --add channels conda-forge

REM install shapeworks deps
call conda create --yes --name %CONDAENV% python=3.12.3 pip=24.3.1 openblas=0.3.30 || goto :error
call conda activate %CONDAENV%

REM reactivate shapeworks environment
call conda activate base
call conda activate %CONDAENV%

call python -m pip install -r python_requirements.txt || goto :error

REM install pytorch using light-the-torch
REM Use python -m to ensure we use the conda env's light_the_torch
call python -m light_the_torch install torch==2.8.0 torchaudio==2.8.0 torchvision==0.23.0 || goto :error

REM different versions of open3d for different OSes, so we install it manually here
call pip install open3d==0.19.0 || goto :error

call pip install Python/DataAugmentationUtilsPackage.tar.gz || goto :error
call pip install Python/DatasetUtilsPackage.tar.gz || goto :error
call pip install Python/MONAILabelPackage.tar.gz || goto :error
call pip install Python/DeepSSMUtilsPackage.tar.gz || goto :error
call pip install Python/DocumentationUtilsPackage.tar.gz || goto :error
call pip install Python/ShapeCohortGenPackage.tar.gz || goto :error
call pip install Python/shapeworks.tar.gz || goto :error


REM install the shapeworks python module and conda [de]activate scripts
call .\Installation\install_python_module.bat || goto :error
call .\Installation\conda_env_setup.bat || goto :error

md "%USERPROFILE%\.shapeworks"
python -c "import sys; print('\n'.join(sys.path))" > "%USERPROFILE%\.shapeworks\python_path_6.8.txt"
python -c "import sys; print(sys.prefix)" > "%USERPROFILE%\.shapeworks\python_home_6.8.txt"
echo %PATH% > "%USERPROFILE%\.shapeworks\path_6.8.txt"
call conda info

REM === Validating installation ===
echo.
echo === Validating installation ===
set VALIDATION_FAILED=0

REM Check Python version
for /f "delims=" %%v in ('python -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')"') do set PYTHON_VERSION=%%v
if "%PYTHON_VERSION%"=="3.12" (
    echo [OK] Python version: %PYTHON_VERSION%
) else (
    echo [FAIL] Python version: %PYTHON_VERSION% ^(expected 3.12^)
    set VALIDATION_FAILED=1
)

REM Check torch
python -c "import torch; print(f'[OK] torch {torch.__version__} (CUDA: {torch.cuda.is_available()})')"
if errorlevel 1 (
    echo [FAIL] torch: failed to import
    set VALIDATION_FAILED=1
)

REM Check shapeworks
python -c "import shapeworks; print('[OK] shapeworks package imported successfully')"
if errorlevel 1 (
    echo [FAIL] shapeworks: failed to import
    set VALIDATION_FAILED=1
)

REM Check DeepSSMUtils
python -c "import DeepSSMUtils; print('[OK] DeepSSMUtils package imported successfully')"
if errorlevel 1 (
    echo [FAIL] DeepSSMUtils: failed to import
    set VALIDATION_FAILED=1
)

REM Check vtk
python -c "import vtk; print(f'[OK] vtk {vtk.vtkVersion.GetVTKVersion()}')"
if errorlevel 1 (
    echo [FAIL] vtk: failed to import
    set VALIDATION_FAILED=1
)

REM Check itk
python -c "import itk; print(f'[OK] itk {itk.Version.GetITKVersion()}')"
if errorlevel 1 (
    echo [FAIL] itk: failed to import
    set VALIDATION_FAILED=1
)

echo ================================
if "%VALIDATION_FAILED%"=="1" (
    echo.
    echo WARNING: Some validation checks failed. Please review the output above.
)

echo "ShapeWorks environment %CONDAENV% is ready to use."
goto :EOF

:error
echo "Installation failed. Please check the error message above."
exit /b %ERRORLEVEL%



