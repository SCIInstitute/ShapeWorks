#
# Installs conda environment for building ShapeWorks
#

if [ -z "$PS1" ]; then
  echo "ERROR: must call this script using \"source ./conda_installs.sh\""
  exit 1
fi

if ! command -v conda 2>/dev/null 1>&2; then
  echo "installing anaconda..."
  if [ "$(uname)" == "Darwin" ]; then
    wget https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh
    bash ./Miniconda3-latest-MacOSX-x86_64.sh
    rm ./Miniconda3-latest-MacOSX-x86_64.sh
  elif [ "$(uname)" == "Linux" ]; then
    wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
    bash ./Miniconda3-latest-Linux-x86_64.sh
    rm ./Miniconda3-latest-Linux-x86_64.sh
  else
    echo "ERROR: unknown OS $(uname)"
    return 1
  fi

  source ~/miniconda3/bin/activate
  conda config --set auto_activate_base false
fi

#update anaconda
conda update --yes -n base -c defaults conda
conda update --yes --all
conda install --yes -c anaconda pip     # needed in sub-environments or the base env's pip will silently install to base
pip install --upgrade pip

#create and activate shapeworks env
CONDAENV=shapeworks
conda create --yes --name $CONDAENV python=3.8
eval "$(conda shell.bash hook)"
conda activate $CONDAENV

#install shapeworks deps
conda install --yes -c anaconda geotiff=1.5.1 libxrandr-devel-cos6-x86_64=1.5.1 libxinerama-devel-cos6-x86_64=1.1.3 libxcursor-devel-cos6-x86_64=1.1.14 libxi-devel-cos6-x86_64=1.7.8
conda install --yes -c conda-forge ncurses=6.1 cmake=3.15.5 xorg-libx11=1.6.9 libuuid=2.32.1 xorg-libsm=1.2.3 openmp=8.0.1
conda install --yes -c anaconda numpy=1.17.4
conda install --yes -c conda-forge colorama=0.4.3
conda install --yes -c https://conda.anaconda.org/simpleitk SimpleITK=1.2.4
conda install --yes -c conda-forge requests=2.22.2

pip install termcolor==1.1.0
pip install matplotlib==3.1.2
pip install -e Python/DatasetUtilsPackage # install the local GirderConnector code as a package

if [ "$(uname)" = "Linux" ]; then
  # install anything that's different on linux
fi

conda info
