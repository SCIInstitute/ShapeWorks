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
call conda create --yes --name %CONDAENV% python=3.9.13 openblas=0.3.20 'vtk=9.1.0=qt*' scikit-learn=1.1.1 pip=22.1.2
call conda activate %CONDAENV%

REM reactivate shapeworks environment
call conda activate base
call conda activate %CONDAENV%

call pip install torch==1.11.0+cpu torchvision==0.12.0+cpu torchaudio===0.11.0 -f https://download.pytorch.org/whl/torch_stable.html


call pip install numpy==1.22.4
call pip install requests==2.27.1
call pip install colorama==0.4.5
call pip install notebook==6.1.5
call pip install trimesh==3.12.6
call pip install termcolor==1.1.0
call pip install grip==4.6.1
call pip install hotelling==0.5.0
call pip install statsmodels==0.13.2
call pip install shapely==1.8.2
call pip install matplotlib==3.5.2
call pip install itk==5.2.1.post1
call pip install itkwidgets==0.32.1
call pip install itk-elastix==0.13.0
call pip install SimpleITK==2.1.1.2
call pip install bokeh==2.4.3
call pip install seaborn==0.11.2

call pip install Python/DatasetUtilsPackage
call pip install Python/DocumentationUtilsPackage
call pip install Python/DataAugmentationUtilsPackage
call pip install Python/DeepSSMUtilsPackage
call pip install Python/ShapeCohortGenPackage

REM install the shapeworks python module and conda [de]activate scripts
call .\Installation\install_python_module.bat
call .\Installation\conda_env_setup.bat

md %USERPROFILE%\.shapeworks
python -c "import sys; print('\n'.join(sys.path))" > %USERPROFILE%\.shapeworks\python_path_6.4.txt
python -c "import sys; print(sys.prefix)" > %USERPROFILE%\.shapeworks\python_home_6.4.txt
echo %PATH% > %USERPROFILE%\.shapeworks\path_6.4.txt
call conda info
