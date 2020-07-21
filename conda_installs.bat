REM update anaconda

call conda config --add channels anaconda
call conda config --add channels conda-forge

call conda update --yes -n base -c defaults conda
call conda update --yes --all

REM create and activate shapeworks env
call conda create --yes --name shapeworks python=3.7
call conda activate shapeworks

REM install shapeworks deps
call conda install --yes colorama=0.4.3
call conda install --yes requests=2.22.0
call conda install --yes openblas=0.3.3
call conda install --yes numpy=1.17.4
call conda install --yes scikit-learn=0.22.1
call conda install --yes gtest=1.10.0 cmake-3.15.5

REM reactivate shapeworks environment
call conda activate base
call conda activate shapeworks

call pip install --upgrade pip
call pip install termcolor==1.1.0
call pip install matplotlib==3.1.2
call pip install itk==5.0.1
call pip install vtk==8.1.2
call pip install mdutils

REM install the local GirderConnector code as a package
call pip install Python/DatasetUtilsPackage
call pip install Python/DocumentationUtilsPackage
call pip install Python/DataAugmentationUtilsPackage
call pip install Python/DeepSSMUtilsPackage
  
call conda info





