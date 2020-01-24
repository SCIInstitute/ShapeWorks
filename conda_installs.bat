REM update anaconda
call conda update --yes -n base -c defaults conda
call conda update --yes --all

REM create and activate shapeworks env
call conda create --yes --name shapeworks python=3.7
call conda activate shapeworks

REM install shapeworks deps
call conda install --yes -c conda-forge colorama=0.4.3
call conda install --yes -c conda-forge requests=2.22.0
call conda install --yes -c conda-forge openblas=0.3.3
call conda install --yes -c conda-forge numpy=1.17.4
call conda install --yes -c https://conda.anaconda.org/simpleitk SimpleITK=1.2.4

REM reactivate shapeworks environment
call conda activate base
call conda activate shapeworks

call pip install --upgrade pip
call pip install termcolor
call pip install matplotlib

REM install the local GirderConnector code as a package
call pip install Python/DatasetUtilsPackage

call conda info





