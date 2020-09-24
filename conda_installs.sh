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

    conda activate
    conda config --set auto_activate_base false
  fi

  # add default channels
  conda deactivate
  conda config --add channels anaconda
  conda config --add channels conda-forge
  
  # update anaconda
  if ! conda update --yes -n base conda; then return 1; fi
  if ! conda update --yes --all; then return 1; fi

  # create and activate shapeworks env
  CONDAENV=shapeworks
  if ! conda create --yes --name $CONDAENV python=3.7.8; then return 1; fi
  eval "$(conda shell.bash hook)"
  if ! conda activate $CONDAENV; then return 1; fi

  # pip is needed in sub-environments or the base env's pip will silently install to base
  if ! conda install --yes pip=20.2.3; then return 1; fi
  if ! python -m pip install --upgrade pip; then return 1; fi

  # shapeworks deps, all platforms
  if ! conda install --yes cmake=3.18.2; then return 1; fi
  if ! conda install --yes boost-cpp=1.74.0; then return 1; fi
  if ! conda install --yes colorama=0.4.3; then return 1; fi
  if ! conda install --yes doxygen=1.8.20; then return 1; fi
  if ! conda install --yes geotiff=1.6.0; then return 1; fi
  if ! conda install --yes git-lfs=2.11.0; then return 1; fi
  if ! conda install --yes gtest=1.10.0; then return 1; fi
  if ! conda install --yes notebook=6.1.4; then return 1; fi
  if ! conda install --yes numpy=1.19.1; then return 1; fi
  if ! conda install --yes openblas=0.3.10; then return 1; fi
  if ! conda install --yes openexr=2.5.3; then return 1; fi
  if ! conda install --yes pybind11=2.5.0; then return 1; fi
  if ! conda install --yes requests=2.24.0; then return 1; fi
  if ! conda install --yes scikit-learn=0.23.2; then return 1; fi
  if ! conda install --yes vtk=9.0.1; then return 1; fi # VTK Python bindings (TODO: see issues #300, #302)

  # shapeworks deps, platform-specific (e.g., for different versions)
  if [[ "$(uname)" == "Linux" ]]; then                                  # Linux
    if ! conda install --yes graphviz=2.42.3; then return 1; fi
    if ! conda install --yes libuuid=2.32.1; then return 1; fi
    if ! conda install --yes ncurses=6.2; then return 1; fi
    if ! conda install --yes openmp=8.0.1; then return 1; fi
    if ! conda install --yes tbb=2020.2; then return 1; fi
    if ! conda install --yes tbb-devel=2020.2; then return 1; fi
    if ! conda install --yes xorg-libxcursor=1.2.0; then return 1; fi
    if ! conda install --yes xorg-libxi=1.7.10; then return 1; fi
    if ! conda install --yes xorg-libxinerama=1.1.4; then return 1; fi
    if ! conda install --yes xorg-libxrandr=1.5.2; then return 1; fi
    if ! conda install --yes xorg-libx11=1.6.12; then return 1; fi
    if ! conda install --yes xorg-libsm=1.2.3; then return 1; fi
  
  elif [[ "$(uname)" == "Darwin" ]]; then                               # OSX
    if ! conda install --yes graphviz=2.42.3; then return 1; fi
    if ! conda install --yes tbb=2019.9; then return 1; fi
    if ! conda install --yes tbb-devel=2019.9; then return 1; fi

  else                                                                  # Windows
    if ! conda install --yes graphviz=2.38.1; then return 1; fi
    if ! conda install --yes tbb=2020.1; then return 1; fi
    if ! conda install --yes tbb-devel=2020.1; then return 1; fi
  fi

  # pip installs, all platforms
  if ! pip install Python/DatasetUtilsPackage; then return 1; fi       # install the local GirderConnector code as a package
  if ! pip install Python/DocumentationUtilsPackage; then return 1; fi # install shapeworks auto-documentation as a package
  if ! pip install fontawesome-markdown==0.2.6; then return 1; fi      # lib for icons in documentation
  if ! pip install grip==4.5.2; then return 1; fi                      # quick markdown preview server
  if ! pip install itk==5.1.1; then return 1; fi                       # ITK Python bindings (TODO: see issues #300, #302)
  if ! pip install matplotlib==3.3.2; then return 1; fi
  if ! pip install mdutils==1.3.0; then return 1; fi                   # lib for writing markdown files (auto-documentation)
  if ! pip install mkdocs==1.1.2; then return 1; fi                    # lib for generating documentation from markdown
  if ! pip install pymdown-extensions==8.0; then return 1; fi          # lib to support checkbox lists in documentation
  if ! pip install termcolor==1.1.0; then return 1; fi

  # pip installs, platform-specific (none as of yet)
  if [[ "$(uname)" == "Linux" ]]; then                                 # Linux
    42;
  elif [[ "$(uname)" == "Darwin" ]]; then                              # OSX
    42;
  else                                                                 # Windows
    42;
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
