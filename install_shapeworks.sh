#
# Installs conda environment for building ShapeWorks
#

SW_MAJOR_VERSION=6.7

# Set up logging
INSTALL_LOG="install_shapeworks_$(date +%Y%m%d_%H%M%S).log"
exec > >(tee -a "$INSTALL_LOG") 2>&1
echo "Logging to $INSTALL_LOG"

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


function install_conda() {
  if ! command -v conda 2>/dev/null 1>&2; then
    echo "Installing Miniconda..."
    if [[ "$(uname)" == "Darwin" ]]; then
	if [[ $(uname -m) == 'arm64' ]]; then
          curl -o /tmp/Miniconda3-latest-MacOSX-arm64.sh https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-arm64.sh
          bash /tmp/Miniconda3-latest-MacOSX-arm64.sh -b
          rm /tmp/Miniconda3-latest-MacOSX-arm64.sh
	else
          curl -o /tmp/Miniconda3-latest-MacOSX-x86_64.sh https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh
          bash /tmp/Miniconda3-latest-MacOSX-x86_64.sh -b
          rm /tmp/Miniconda3-latest-MacOSX-x86_64.sh
	fi
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

  CONDA_PACKAGES=(python=3.12 \
    openblas=0.3.30 \
    pip=24.3.1
   )		      

  if [[ "$DEVELOPER" == "YES" ]] ; then
    echo "Developer packages enabled"

    CONDA_PACKAGES+=(cmake=3.23.2 \
		     doxygen=1.9.2 \
		     graphviz=4.0.0 \
		     eigen=3.4.0 \
		     hdf5=1.12.2 \
		     tbb=2021.5.0 \
		     tbb-devel=2021.5.0 \
		     boost=1.85.0 \
		     openexr=3.1.5 \
		     ilmbase=2.5.5 \
		     pybind11=3.0.1 \
		     nlohmann_json=3.10.5 \
		     spdlog=1.10.0 \
		     pkg-config=0.29.2 \
		     openh264==2.3.0 \
		     libhwloc=2.8.0 \
		     qt-main=5.15.8 \
		    )
    
    # linux (only) deps
    if [[ "$(uname)" == "Linux" ]]; then
	# required by install_python_module.sh
	CONDA_PACKAGES+=(zlib=1.2.13 patchelf=0.17.2)
    fi

    if [[ $OSTYPE == "msys" ]]; then
	CONDA_PACKAGES+=(zlib)
    fi
    
  fi

  echo "Installing CONDA_PACKAGES = ${CONDA_PACKAGES[@]}"

  # Accept Anaconda TOS for required channels
  conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/main
  conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/r

  # create and activate shapeworks env
  if ! conda create --yes --name $CONDAENV ${CONDA_PACKAGES[@]} ; then
      return 1;
  fi

  eval "$(conda shell.bash hook)"
  if ! conda activate $CONDAENV; then return 1; fi

  echo "=== Environment after first activation ==="
  echo "CONDA_PREFIX: $CONDA_PREFIX"
  echo "which python: $(which python)"
  echo "============================================"

  # install conda into the shell
  conda init

  # re-activate after conda init (it can disrupt the environment)
  eval "$(conda shell.bash hook)"
  conda activate $CONDAENV

  echo "=== Environment after re-activation ==="
  echo "CONDA_PREFIX: $CONDA_PREFIX"
  echo "which python: $(which python)"
  echo "PATH: $PATH"
  echo "========================================"

  if ! python -m pip install -r python_requirements.txt;          then return 1; fi

  # install pytorch using light-the-torch
  # Use python -m to ensure we use the conda env's light_the_torch, not ~/.local/bin/ltt
  echo "=== PyTorch installation diagnostics ==="
  echo "CONDA_PREFIX: $CONDA_PREFIX"
  echo "which python: $(which python)"
  echo "python version: $(python --version)"
  echo "python sys.executable: $(python -c 'import sys; print(sys.executable)')"
  echo "which pip: $(which pip)"
  echo "which ltt: $(which ltt)"
  echo "========================================="
  if [[ $(uname -s) == "Darwin" ]] && [[ $(uname -m) == "x86_64" ]]; then
    # Intel Mac - use older versions with NumPy 1.x
    if ! python -m light_the_torch install torch==2.2.2 torchaudio==2.2.2 torchvision==0.17.2; then return 1; fi
    pip install "numpy<2"
  else
    # Apple Silicon, Linux, Windows - use latest with NumPy 2.x support
    if ! python -m light_the_torch install torch==2.8.0 torchaudio==2.8.0 torchvision==0.23.0; then return 1; fi
  fi

  # for network analysis
  # open3d needs to be installed differently on each platform so it's not part of python_requirements.txt
  if [[ "$(uname)" == "Linux" ]]; then
      if ! pip install open3d-cpu==0.19.0;       then return 1; fi
  elif [[ "$(uname)" == "Darwin" ]]; then
      if ! pip install open3d==0.19.0;           then return 1; fi
      
      if [[ "$(uname -m)" == "arm64" ]]; then
        pushd $CONDA_PREFIX/lib/python3.12/site-packages/open3d/cpu
        install_name_tool -change /opt/homebrew/opt/libomp/lib/libomp.dylib @rpath/libomp.dylib pybind.cpython-312-darwin.so
        install_name_tool -add_rpath @loader_path/../../../ pybind.cpython-312-darwin.so
        popd
        ln -sf "$CONDA_PREFIX/lib/libomp.dylib" "$CONDA_PREFIX/lib/python3.12/site-packages/open3d/cpu/../../../libomp.dylib"
      fi
  else
      if ! pip install open3d==0.19.0;           then return 1; fi
  fi

  for package in DataAugmentationUtilsPackage DatasetUtilsPackage MONAILabelPackage DeepSSMUtilsPackage DocumentationUtilsPackage ShapeCohortGenPackage shapeworks ; do
    if [[ -e Python/${package}.tar.gz ]] ; then
      if ! pip install Python/${package}.tar.gz;        then return 1; fi
    else
      if ! pip install Python/${package};               then return 1; fi
    fi
  done


  echo "Checking for darwin"
  if [[ "$(uname)" == "Darwin" ]]; then
    echo "Checking for arm64"
    if [[ "$(uname -m)" == "arm64" ]]; then
      echo "copying file to fix!"
      # fix for broken packages that overwrite itk/__init__.py
      echo cp Support/itk-arm64-fix $CONDA_PREFIX/lib/python3.12/site-packages/itk/__init__.py
      cp Support/itk-arm64-fix $CONDA_PREFIX/lib/python3.12/site-packages/itk/__init__.py
    fi
  fi
  
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

  echo "Conda info:"
  conda info

  echo "Conda installed packages:"
  conda list

  echo "Pip installed packages:"
  pip list

  conda clean -t -y

  conda activate $CONDAENV

  # Validate installation
  echo ""
  echo "=== Validating installation ==="
  VALIDATION_FAILED=0

  # Check Python version
  PYTHON_VERSION=$(python -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
  if [[ "$PYTHON_VERSION" == "3.12" ]]; then
    echo "✓ Python version: $PYTHON_VERSION"
  else
    echo "✗ Python version: $PYTHON_VERSION (expected 3.12)"
    VALIDATION_FAILED=1
  fi

  # Check torch is installed and using correct Python
  if python -c "import torch; print(f'✓ torch {torch.__version__} (CUDA: {torch.cuda.is_available()})')" 2>/dev/null; then
    TORCH_PATH=$(python -c "import torch; print(torch.__file__)")
    if [[ "$TORCH_PATH" == *"$CONDAENV"* ]]; then
      echo "  installed in conda env: $TORCH_PATH"
    else
      echo "✗ torch installed in wrong location: $TORCH_PATH"
      VALIDATION_FAILED=1
    fi
  else
    echo "✗ torch: failed to import"
    VALIDATION_FAILED=1
  fi

  # Check shapeworks package (may not be available in developer mode until built)
  if python -c "import shapeworks; print(f'✓ shapeworks package imported successfully')" 2>/dev/null; then
    :
  else
    if [[ "$DEVELOPER" == "YES" ]]; then
      echo "- shapeworks: not yet available (expected in developer mode, will be available after build)"
    else
      echo "✗ shapeworks: failed to import"
      VALIDATION_FAILED=1
    fi
  fi

  # Check DeepSSMUtils (may not be available in developer mode until built)
  if python -c "import DeepSSMUtils; print(f'✓ DeepSSMUtils package imported successfully')" 2>/dev/null; then
    :
  else
    if [[ "$DEVELOPER" == "YES" ]]; then
      echo "- DeepSSMUtils: not yet available (expected in developer mode, will be available after build)"
    else
      echo "✗ DeepSSMUtils: failed to import"
      VALIDATION_FAILED=1
    fi
  fi

  # Check vtk
  if python -c "import vtk; print(f'✓ vtk {vtk.vtkVersion.GetVTKVersion()}')" 2>/dev/null; then
    :
  else
    echo "✗ vtk: failed to import"
    VALIDATION_FAILED=1
  fi

  # Check itk
  if python -c "import itk; print(f'✓ itk {itk.Version.GetITKVersion()}')" 2>/dev/null; then
    :
  else
    echo "✗ itk: failed to import"
    VALIDATION_FAILED=1
  fi

  echo "================================"

  if [[ "$VALIDATION_FAILED" == "1" ]]; then
    echo ""
    echo "WARNING: Some validation checks failed. Please review the output above."
  else
    echo ""
    echo "$CONDAENV environment successfully created/updated!"
  fi

else
  echo "Problem encountered creating/updating $CONDAENV conda environment."
  return 1;
fi

