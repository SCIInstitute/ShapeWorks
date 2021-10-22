REM remove existing shapeworks env if any
set CONDAENV=shapeworks
IF NOT "%1"=="" SET CONDAENV=%1
echo "creating new conda environment for ShapeWorks called %CONDAENV%..."

REM remove any existing environment with this name
call conda env remove -n %CONDAENV%

REM update anaconda
call conda config --add channels anaconda
call conda config --add channels conda-forge

call conda create --yes --name %CONDAENV% python=3.7
call conda activate %CONDAENV%

REM install shapeworks deps
call conda install --yes colorama=0.4.3
call conda install --yes requests=2.22.0
call conda install --yes openblas=0.3.3
call conda install --yes numpy=1.17.4
call conda install --yes scikit-learn=0.22.1
call conda install --yes trimesh
call conda install --yes gtest=1.10.0 cmake-3.15.5
call conda install --yes pytorch torchvision cpuonly -c pytorch
call conda install --yes notebook=6.1.5

REM reactivate shapeworks environment
call conda activate base
call conda activate %CONDAENV%

call conda install --yes pip=21.2.4
call pip install termcolor==1.1.0
call pip install matplotlib==3.1.2
call pip install hotelling==0.5.0
call pip install statsmodels
call pip install itk==5.0.1
call pip install vtk==8.1.2
call pip install bokeh==2.2.0
call pip install shapely
call pip install seaborn
call pip install mdutils
call pip install mkdocs
call pip install python-markdown-math
call pip install fontawesome-markdown
call pip install pymdown-extensions

REM install the local GirderConnector code as a package
call pip install Python/DatasetUtilsPackage
call pip install Python/DocumentationUtilsPackage
call pip install Python/DataAugmentationUtilsPackage
call pip install Python/DeepSSMUtilsPackage
call pip install Python/ShapeCohortGenPackage

REM install the shapeworks python module and conda [de]activate scripts
call .\Installation\install_python_module.bat
call .\Installation\conda_env_setup.bat

REM installs for jupyter notebooks
call pip install nbstripout
call pip install pyvista==0.30.1
call pip install ipyvtklink
call pip install ipyvtk_simple
call pip install ipywidgets
call pip install itkwidgets
call pip install mkdocs-jupyter

REM for spell check markdown cells in jupyter notebooks and table of content
call conda install --yes jupyter_contrib_nbextensions
call jupyter contrib nbextension install --user
call jupyter nbextension enable spellchecker/main
call jupyter nbextension enable toc2/main

md %USERPROFILE%\.shapeworks
python -c "import sys; print('\n'.join(sys.path))" > %USERPROFILE%\.shapeworks\python_path.txt
echo %PATH% > %USERPROFILE%\.shapeworks\path.txt
call conda info
