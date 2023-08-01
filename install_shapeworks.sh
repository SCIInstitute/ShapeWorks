#
# Installs conda environment for building ShapeWorks
#

SW_MAJOR_VERSION=6.5

echo ""
echo "Note: this script only supports bash and zsh shells "
echo "      It must be called using \"source ./install_shapeworks.sh [--developer] [--user] [optional_env_name]\""
echo ""

(return 0 2>/dev/null) && sourced=1 || sourced=0

if [[ "$sourced" == "0" ]]; then
  echo "ERROR: must call this script using \"source ./install_shapeworks.sh [optional_env_name]\""
  exit 1
fi


DEVELOPER=NO
if [ -d ".git" ]; then # default to developer if in a git repo
    DEVELOPER=YES
fi
    
POSITIONAL_ARGS=()
while [[ $# -gt 0 ]]; do
  case $1 in
    --developer)
      DEVELOPER=YES
      shift # past argument
      ;;
    --user)
      DEVELOPER=NO
      shift # past argument
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

CONDAENV=shapeworks
if [[ "${#POSITIONAL_ARGS[@]}" -eq 1 ]]; then
   CONDAENV=${POSITIONAL_ARGS[@]:0:1}
fi

echo "Creating new conda environment for ShapeWorks called \"$CONDAENV\"..."


# PyTorch installation
function install_pytorch() {
  echo "installing pytorch"
  if [[ "$(uname)" == "Darwin" ]]; then
    pip install torch==1.11.0 torchvision==0.12.0 torchaudio==0.11.0
  elif ! [ -x "$(command -v nvidia-smi)" ]; then
    echo 'Could not find nvidia-smi, using cpu-only PyTorch'
    pip install torch==1.11.0+cpu torchvision==0.12.0+cpu torchaudio==0.11.0 -f https://download.pytorch.org/whl/torch_stable.html
  else
    CUDA=`nvidia-smi | grep CUDA | sed -e "s/.*CUDA Version: //" -e "s/ .*//"`
    echo "Found CUDA Version: ${CUDA}"
    if [[ "$CUDA" == "9.2" ]]; then
        pip install torch==1.11.0+cu92 torchvision==0.12.0+cu92 torchaudio==0.11.0 -f https://download.pytorch.org/whl/torch_stable.html
    elif [[ "$CUDA" == "10.1" ]]; then
        pip install torch==1.11.0+cu101 torchvision==0.12.0+cu101 torchaudio==0.11.0 -f https://download.pytorch.org/whl/torch_stable.html
    elif [[ "$CUDA" == "10.2" ]]; then
        pip install torch==1.11.0 torchvision==0.12.0 torchaudio==0.11.0 -f https://download.pytorch.org/whl/torch_stable.html
    elif [[ "$CUDA" == "11.0" || "$CUDA" == "11.1" || "$CUDA" == "11.2" ]]; then
        pip install torch==1.11.0+cu110 torchvision==0.12.0+cu110 torchaudio==0.11.0 -f https://download.pytorch.org/whl/torch_stable.html
    elif [[ "$CUDA" == "11.7" || "$CUDA" == "11.8" || "$CUDA" == "12.0" || "$CUDA" == "12.1" ]]; then
	pip install torch==1.13.1 torchvision==0.14.1 torchaudio==0.13.1 -f https://download.pytorch.org/whl/cu118
    else
        echo "CUDA version not compatible, using cpu-only"
        pip install torch==1.11.0+cpu torchvision==0.12.0+cpu torchaudio==0.11.0 -f https://download.pytorch.org/whl/torch_stable.html
    fi
  fi
}

function install_conda() {
  if ! command -v conda 2>/dev/null 1>&2; then
    echo "Installing Miniconda..."
    if [[ "$(uname)" == "Darwin" ]]; then
      curl -o /tmp/Miniconda3-latest-MacOSX-x86_64.sh https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh
      bash /tmp/Miniconda3-latest-MacOSX-x86_64.sh -b
      rm /tmp/Miniconda3-latest-MacOSX-x86_64.sh
    elif [[ "$(uname)" == "Linux" ]]; then
      curl -o ./Miniconda3-latest-Linux-x86_64.sh https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
      bash ./Miniconda3-latest-Linux-x86_64.sh -b
      rm ./Miniconda3-latest-Linux-x86_64.sh
    else
      echo "ERROR: unknown OS $(uname)"
      return 1
    fi

    source ~/miniconda3/bin/activate
    conda config --set auto_activate_base false
  fi

  # add default channels
  conda config --add channels anaconda
  conda config --add channels conda-forge

  CONDA_PACKAGES=(python=3.9.13 \
    openblas=0.3.20 \
    pip=22.1.2
   )		      

  if [[ "$DEVELOPER" == "YES" ]] ; then
    echo "Developer packages enabled"

    CONDA_PACKAGES+=(cmake=3.23.2 \
		     gmock=1.11.0 \
		     doxygen=1.9.2 \
		     graphviz=4.0.0 \
		     eigen=3.4.0 \
		     hdf5=1.12.2 \
		     tbb=2021.5.0 \
		     tbb-devel=2021.5.0 \
		     boost=1.74.0 \
		     openexr=3.1.5 \
		     ilmbase=2.5.5 \
		     pybind11=2.9.2 \
		     nlohmann_json=3.10.5 \
		     spdlog=1.10.0 \
		     pkg-config=0.29.2 \
		     openh264==2.3.0 \
		     libhwloc=2.8.0 \
		     qt-main=5.15.4 \
		    )
    
    # linux (only) deps
    if [[ "$(uname)" == "Linux" ]]; then
	# required by install_python_module.sh
	CONDA_PACKAGES+=(zlib=1.2.12 patchelf=0.14.5)
    fi
  fi

  echo "Installing CONDA_PACKAGES = ${CONDA_PACKAGES[@]}"

  # create and activate shapeworks env
  if ! conda create --yes --name $CONDAENV ${CONDA_PACKAGES[@]} ; then
      return 1;
  fi

  eval "$(conda shell.bash hook)"
  if ! conda activate $CONDAENV; then return 1; fi
  
  # install conda into the shell
  conda init

  pip install -r python_requirements.txt

  # for network analysis
  if [[ "$(uname)" == "Linux" ]]; then
      if ! pip install open3d-cpu==0.17.0;              then return 1; fi
  else
      if ! pip install open3d==0.17.0;                  then return 1; fi
  fi

  for package in DataAugmentationUtilsPackage DatasetUtilsPackage DeepSSMUtilsPackage DocumentationUtilsPackage ShapeCohortGenPackage shapeworks ; do
    if [[ -e Python/${package}.tar.gz ]] ; then
      if ! pip install Python/${package}.tar.gz;        then return 1; fi
    else
      if ! pip install Python/${package};               then return 1; fi
    fi
  done
  
  ./Installation/install_python_module.sh   # install python module
  ./Installation/conda_env_setup.sh         # install conda [de]activate scripts


  if [[ "$GITHUB_ACTION" != "" ]]; then
    if [[ "$(uname)" == "Linux" ]]; then

      echo "Running under GitHub Action, creating libffi link"
      pushd $HOME/miniconda3/envs/shapeworks/lib
      ls libffi*
      if [ ! -f libffi.6.dylib ]; then
	  ln -s libffi.7.dylib libffi.6.dylib
      fi
      popd
    fi
  fi

  # for spell check markdown cells in jupyter notebooks and table of contents (toc2)
  jupyter contrib nbextension install --user
  jupyter nbextension enable spellchecker/main
  jupyter nbextension enable toc2/main

  if [[ "$DEVELOPER" == "YES" ]] ; then
    # installing nbstripout to strip out notebooks cell outputs before committing 
    nbstripout --install
    nbstripout --install --attributes .gitattributes
  fi

  # Set the python path for studio
  conda activate $CONDAENV
  mkdir -p $HOME/.shapeworks
  python -c "import sys; print('\n'.join(sys.path))" > $HOME/.shapeworks/python_path_${SW_MAJOR_VERSION}.txt
  python -c "import sys; print(sys.prefix)" > $HOME/.shapeworks/python_home_${SW_MAJOR_VERSION}.txt

  return 0
}

if install_conda; then
  install_pytorch

  echo "Conda info:"
  conda info

  echo "Conda installed packages:"
  conda list

  echo "Pip installed packages:"
  pip list

  conda clean -t -y
  
  echo "$CONDAENV environment successfully created/updated!"
  
  conda activate $CONDAENV
else
  echo "Problem encountered creating/updating $CONDAENV conda environment."
  return 1;
fi

