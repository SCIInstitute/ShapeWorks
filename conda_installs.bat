REM update anaconda

call conda config --add channels anaconda
call conda config --add channels conda-forge

call conda update --yes -n base -c defaults conda
call conda update --yes --all

REM create and activate shapeworks env
call conda create --yes --name shapeworks python=3.7.8
call conda activate shapeworks

REM install shapeworks deps
call conda install --yes colorama=0.4.3
call conda install --yes requests=2.24.0
call conda install --yes openblas=0.3.6
call conda install --yes numpy=1.19.1
call conda install --yes scikit-learn=0.22.1
call conda install --yes gtest=1.10.0 cmake-3.18.2
call conda install --yes pybind11=2.5.0 notebook=6.1.4

REM reactivate shapeworks environment
call conda activate base
call conda activate shapeworks

call pip install --upgrade pip
call pip install termcolor==1.1.0
call pip install matplotlib==3.3.2
call pip install itk==5.1.1
call pip install vtk==9.0.1
call pip install mdutils==1.3.0
call pip install mkdocs==1.1.2
call pip install fontawesome-markdown==0.2.6
call pip install pymdown-extensions==8.0

REM install the local GirderConnector code as a package
call pip install Python/DatasetUtilsPackage
call pip install Python/DocumentationUtilsPackage
  
call conda info





