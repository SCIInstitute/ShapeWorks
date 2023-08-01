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
call conda create --yes --name %CONDAENV% python=3.9.13 pip=22.1.2
call conda activate %CONDAENV%

REM reactivate shapeworks environment
call conda activate base
call conda activate %CONDAENV%

call pip install torch==1.11.0+cpu torchvision==0.12.0+cpu torchaudio===0.11.0 -f https://download.pytorch.org/whl/torch_stable.html

call pip install -r python_requirements.txt

call pip install Python/DatasetUtilsPackage.tar.gz
call pip install Python/DocumentationUtilsPackage.tar.gz
call pip install Python/DataAugmentationUtilsPackage.tar.gz
call pip install Python/DeepSSMUtilsPackage.tar.gz
call pip install Python/ShapeCohortGenPackage.tar.gz
call pip install Python/shapeworks.tar.gz


REM install the shapeworks python module and conda [de]activate scripts
call .\Installation\install_python_module.bat
call .\Installation\conda_env_setup.bat

md "%USERPROFILE%\.shapeworks"
python -c "import sys; print('\n'.join(sys.path))" > "%USERPROFILE%\.shapeworks\python_path_6.5.txt"
python -c "import sys; print(sys.prefix)" > "%USERPROFILE%\.shapeworks\python_home_6.5.txt"
echo %PATH% > "%USERPROFILE%\.shapeworks\path_6.5.txt"
call conda info
