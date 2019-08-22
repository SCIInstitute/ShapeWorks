#
# Installs conda environment for building ShapeWorks
#

if [ -z "$PS1" ]; then
  echo "ERROR: must call this script using \"source ./conda_installs.sh\")"
  exit 1
fi

if [[ ! 'command -v conda' ]]; then
  wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
  bash ./Miniconda3-latest-MacOSX-x86_64.sh
  source ~/miniconda3/bin/activate
  conda config --set auto_activate_base false
  rm ./Miniconda3-latest-MacOSX-x86_64.sh
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
conda install --yes numpy matplotlib colorama
pip install termcolor

#optionally install conda Qt (standalone version recommended)
if [[ INSTALL_QT ]]; then
  conda install --yes -c anaconda qt
fi

conda info

