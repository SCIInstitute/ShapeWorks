REM update anaconda
call conda update --yes -n base -c defaults conda
call conda update --yes --all

REM create and activate shapeworks env
call conda create --yes --name shapeworks python=3.5
call conda activate shapeworks

REM install shapeworks deps
REM conda install --yes -c conda-forge cmake xorg-libx11 libuuid xorg-libsm 
REM conda install --yes -c anaconda numpy
call conda install --yes -c conda-forge colorama
call conda install --yes -c https://conda.anaconda.org/simpleitk SimpleITK
call conda install --yes -c conda-forge requests
call pip install --upgrade pip
call pip install termcolor
call pip install matplotlib

REM install the local GirderConnector code as a package
call pip install -e Python/DatasetUtilsPackage

call conda activate base
call conda activate shapeworks

call conda info





