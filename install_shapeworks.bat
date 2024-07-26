REM remove existing shapeworks env if any
set CONDAENV=shapeworks
IF NOT "%1"=="" SET CONDAENV=%1
echo "creating new conda environment for ShapeWorks called %CONDAENV%..."

REM remove any existing environment with this name
call conda env remove -n %CONDAENV%

REM update anaconda
call conda config --add channels anaconda
call conda config --add channels conda-forge

REM install shapeworks deps
call conda create --yes --name %CONDAENV% python=3.9.13 pip=22.1.2 || goto :error
call conda activate %CONDAENV%

REM reactivate shapeworks environment
call conda activate base
call conda activate %CONDAENV%

call python -m pip install -r python_requirements.txt || goto :error

REM install ptorch using light-the-torch
call ltt install torch==1.11.0 || goto :error

REM different versions of open3d for different OSes, so we install it manually here
call pip install open3d==0.17.0 || goto :error

call pip install Python/DatasetUtilsPackage.tar.gz || goto :error
call pip install Python/DocumentationUtilsPackage.tar.gz || goto :error
call pip install Python/DataAugmentationUtilsPackage.tar.gz || goto :error
call pip install Python/DeepSSMUtilsPackage.tar.gz || goto :error
call pip install Python/ShapeCohortGenPackage.tar.gz || goto :error
call pip install Python/shapeworks.tar.gz || goto :error


REM install the shapeworks python module and conda [de]activate scripts
call .\Installation\install_python_module.bat || goto :error
call .\Installation\conda_env_setup.bat || goto :error

md "%USERPROFILE%\.shapeworks"
python -c "import sys; print('\n'.join(sys.path))" > "%USERPROFILE%\.shapeworks\python_path_6.6.txt"
python -c "import sys; print(sys.prefix)" > "%USERPROFILE%\.shapeworks\python_home_6.6.txt"
echo %PATH% > "%USERPROFILE%\.shapeworks\path_6.6.txt"
call conda info

echo "ShapeWorks environment %CONDAENV% is ready to use."
goto :EOF

:error
echo "Installation failed. Please check the error message above."
exit /b %ERRORLEVEL%



