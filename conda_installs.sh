#
# Installs conda environment for building ShapeWorks
#

if [ -z "$PS1" ]; then
  echo "ERROR: must call this script using \"source ./conda_installs.sh\")"
  exit 1
fi

if ! [ -x "$(command -v conda)" ]; then
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

#create and activate shapeworks env
conda create --yes --name shapeworks python=3.5
eval "$(conda shell.bash hook)"
conda activate shapeworks

#install shapeworks deps
conda install --yes -c anaconda cmake geotiff libxrandr-devel-cos6-x86_64 libxinerama-devel-cos6-x86_64 libxcursor-devel-cos6-x86_64 libxi-devel-cos6-x86_64
conda install --yes -c conda-forge xorg-libx11 libuuid xorg-libsm 
conda install --yes -c anaconda numpy
conda install --yes -c conda-forge matplotlib
conda install --yes -c conda-forge colorama
conda install --yes -c https://conda.anaconda.org/simpleitk SimpleITK
pip install termcolor

conda info





