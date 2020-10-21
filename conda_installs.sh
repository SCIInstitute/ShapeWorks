#
# Installs conda environment for building ShapeWorks
#
echo ""
echo "Note: this script only supports bash and zsh shells"
echo ""

(return 0 2>/dev/null) && sourced=1 || sourced=0

if [[ "$sourced" == "0" ]]; then
  echo "ERROR: must call this script using \"source ./conda_installs.sh\""
  exit 1
fi

# PyTorch installation
function install_pytorch() {
  echo "installing pytorch"
  PYTORCH="cpuonly"
  if ! [ -x "$(command -v nvidia-smi)" ]; then
    echo 'Could not find nvidia-smi, using cpu-only PyTorch'
  else
    CUDA=`nvidia-smi | grep CUDA | sed -e "s/.*CUDA Version: //" -e "s/ .*//"`
    echo "Found CUDA Version: ${CUDA}"

    if [[ "$CUDA" == "9.2" || "$CUDA" == "10.1" || "$CUDA" == "10.2" ]]; then
        PYTORCH="cudatoolkit=${CUDA}"
    else
        echo "CUDA version not compatible, using cpu-only"
    fi
  fi

  conda install --yes pytorch torchvision $PYTORCH -c pytorch
}

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
  if ! python -m pip install --upgrade pip; then return 1; fi

  #install shapeworks deps
  if ! conda install --yes \
       cmake=3.15.5 \
       gtest=1.10.0 \
       colorama=0.4.3 \
       requests=2.22.0 \
       geotiff=1.5.1 \
       numpy=1.17.4 \
       git-lfs=2.6.1 \
       openblas=0.3.3 \
       doxygen=1.8.16 \
       graphviz=2.38.0 \
       vtk=8.2.0 \
       scikit-learn=0.22.1 \
       tbb=2019.9 \
       tbb-devel=2019.9 \
       boost=1.72.0 \
       openexr=2.4.1
  then return 1; fi


  # linux and mac (only) deps
  if [[ "$(uname)" == "Linux" || "$(uname)" == "Darwin" ]]; then
      if ! conda install --yes \
	   xorg-libx11=1.6.9 \
	   xorg-libsm=1.2.3 \
	   libxrandr-devel-cos6-x86_64=1.5.1 \
	   libxinerama-devel-cos6-x86_64=1.1.3 \
	   libxcursor-devel-cos6-x86_64=1.1.14 \
	   libxi-devel-cos6-x86_64=1.7.8 \
	   git-lfs=2.6.1 \
	   openmp=8.0.1 \
	   ncurses=6.1 \
	   libuuid=2.32.1
      then return 1; fi
  fi

  
  if ! pip install termcolor==1.1.0; then return 1; fi
  if ! pip install grip==4.5.2; then return 1; fi
  if ! pip install matplotlib==3.1.2; then return 1; fi
  if ! pip install itk==5.0.1; then return 1; fi
  if ! pip install bokeh==2.2; then return 1; fi
  if ! pip install mdutils; then return 1; fi # lib for writing markdown files needed for auto-documentation (not available through conda install)
  if ! pip install mkdocs; then return 1; fi # lib for generating documentation from markdown
  if ! pip install python-markdown-math; then return 1; fi # lib for rendering equations in docs
  if ! pip install fontawesome-markdown; then return 1; fi # lib for icons in documentation
  if ! pip install --upgrade pymdown-extensions; then return 1; fi # lib to support checkbox lists in documentation
  if ! pip install Python/DatasetUtilsPackage; then return 1; fi   # install the local GirderConnector code as a package
  if ! pip install Python/DocumentationUtilsPackage; then return 1; fi   # install shapeworks auto-documentation as a package
  if ! pip install Python/DataAugmentationUtilsPackage; then return 1; fi   # install data augmentation code as a package
  if ! pip install Python/DeepSSMUtilsPackage; then return 1; fi   # install DeepSSM code as a package

  # install any additional Linux dependencies
  if [[ "$(uname)" == "Linux" ]]; then
    echo "nothing additional to install for Linux"
  fi

  conda info
  return 0
}

if install_conda; then
  install_pytorch
  echo "$CONDAENV environment successfully created/updated!"
  conda activate $CONDAENV
else
  echo "Problem encountered creating/updating $CONDAENV conda environment."
  return 1;
fi