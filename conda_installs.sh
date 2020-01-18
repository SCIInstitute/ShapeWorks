#
# Installs conda environment for building ShapeWorks
#

(return 0 2>/dev/null) && sourced=1 || sourced=0

if [ "$sourced" == "0" ]; then
  echo "ERROR: must call this script using \"source ./conda_installs.sh\""
  exit 1
fi

function install_conda() {
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
  conda config --add channels simpleitk
  conda config --add channels anaconda
  conda config --add channels conda-forge
  
  if ! conda update --yes -n base conda; then return 1; fi
  if ! conda update --yes --all; then return 1; fi

  #create and activate shapeworks env
  CONDAENV=shapeworks
  if ! conda create --yes --name $CONDAENV python=3.7; then return 1; fi
  eval "$(conda shell.bash hook)"
  if ! conda activate $CONDAENV; then return 1; fi
  
  # pip is needed in sub-environments or the base env's pip will silently install to base
  if ! conda install --yes pip; then return 1; fi
  if ! pip install --upgrade pip; then return 1; fi


  
  
  #install shapeworks deps
  if ! conda install --yes \
       cmake=3.15.5 \
       xorg-libx11=1.6.9 \
       xorg-libsm=1.2.3 \
       openmp=8.0.1 \
       colorama=0.4.3 \
       requests=2.22.0 \
       libxrandr-devel-cos6-x86_64=1.5.1 \
       libxinerama-devel-cos6-x86_64=1.1.3 \
       libxcursor-devel-cos6-x86_64=1.1.14 \
       libxi-devel-cos6-x86_64=1.7.8 \
       geotiff=1.5.1 \
       numpy=1.17.4 \
       git-lfs=2.6.1 \
       SimpleITK=1.2.4 \
       openblas=0.3.3
  then return 1; fi

  # linux and mac deps
  if [ "$(uname)" = "Linux" ] || [ "$(uname)" == "Darwin"]; then
      if ! conda install --yes \
	   ncurses=6.1 \
	   libuuid=2.32.1
      then return 1; fi
  fi
  
  if ! pip install termcolor==1.1.0; then return 1; fi
  if ! pip install matplotlib==3.1.2; then return 1; fi
  if ! pip install -e Python/DatasetUtilsPackage; then return 1; fi   # install the local GirderConnector code as a package

  # install any additional Linux dependencies
  if [ "$(uname)" = "Linux" ]; then
    echo "nothing additional to install for Linux"
  fi

  conda info
  return 0
}

if install_conda; then
  echo "$CONDAENV environment successfully created/updated!"
  conda activate $CONDAENV
else
  echo "Problem encountered creating/updating $CONDAENV conda environment."
  return 1;
fi

