#!/bin/bash

#set -v   #verbose execution for debugging
#set -x   #tracing execution for debugging (echos all commands from script)

# defaults
BUILD_CLEAN=0
NUM_PROCS=8
BUILD_GUI=1
BUILD_STUDIO=0
BUILD_SHAPEWORKS=1
BUILD_LOG="shapeworks_superbuild.log"
VXL_VER="v2.0.2"
VTK_VER="v8.2.0"
VTK_VER_STR="8.2"
ITK_VER="v5.0.1"
ITK_VER_STR="5.0"
QT_MIN_VER="5.9.8"  # NOTE: 5.x is required, but this restriction is a clever way to ensure the anaconda version of Qt (5.9.6 or 5.9.7) isn't used since it won't work on most systems.

usage()
{
  echo "usage: ./superbuild.sh [[-n=<num-procs>] [-i=<install_path>] [-b=<build_path>] [--clean] [--no-gui] [--vxl-dir=<vxl_path>] [--vtk-dir=<vtk_path>] [--itk-dir=<itk_path>] | [-h | --help]]"
  echo ""
  echo "If using Anaconda to install prerequisites please first run:"
  echo "source ./conda_installs.sh"
  echo ""
  echo "For GUI applications, please make sure at least version $QT_MIN_VER of Qt5 is installed and that its qmake is in the path."
  echo "Download Qt5 from: https://download.qt.io/archive/qt/"
  echo ""
  echo "Arguments:"
  echo "  -h,--help               : Show this screen."
  echo "  --clean                 : Remove all build directories and clone implicit dependencies."
  echo "                          : (note that user-specified paths such as --itk-dir=<path> will not be deleted)."
  echo "  --no-gui                : Do not build the ShapeWorks gui applicaitons, which require Qt."
  echo "                          : The GUI is built by default if qmake >= $QT_MIN_VER is found in the path."
  echo "  --with-studio           : Build ShapeWorksStudio (default off)."
  echo "  -b,--build-dir=<path>   : Build directory for ShapeWorks and its implicit dependencieas (VXL, VTK, and ITK)."
  echo "                          : By default uses a subdirectory of the current directory called 'build'."
  echo "  -i,--install-dir=<path> : Install directory for ShapeWorks and its implicit dependencieas (VXL, VTK, and ITK)."
  echo "                          : By default uses a subdirectory of the current directory called 'install'."
  echo "  -n,--num-procs=<num>    : Number of processors to use for parallel builds (default is 8)."
  echo "  --vxl-dir=<path>        : Path to existing VXL installation (version >= ${VXL_VER})."
  echo "  --vtk-dir=<path>        : Path to existing VTK installation (version >= ${VTK_VER})."
  echo "  --itk-dir=<path>        : Path to existing ITK installation (version >= ${ITK_VER})."
  echo ""
  echo "  --dependencies_only     : Only build dependencies, not ShapeWorks."
  echo ""
  echo "Example: ./superbuild.sh --num-procs=8 --install-dir=/path/to/desired/installation"
  echo "Build results are saved in ${BUILD_LOG}."
}

# print full names of relative paths, which are needed to specify location of dependencies
function realpath { echo $(cd $(dirname $1); pwd)/$(basename $1); }

parse_command_line()
{
  while [ "$1" != "" ]; do
    case $1 in
      -i=*|--install-dir=*)   INSTALL_DIR="${1#*=}"
                              INSTALL_DIR=`realpath "${INSTALL_DIR}"`
                              ;;
      -b=*|--build-dir=*)     BUILD_DIR="${1#*=}"
                              BUILD_DIR=`realpath "${BUILD_DIR}"`
                              ;;
      -n=*|--num-procs=*)     NUM_PROCS="${1#*=}"
                              ;;
      --no-gui )              BUILD_GUI=0
                              ;;
      --with-studio )         BUILD_STUDIO=1
                              ;;
      --vxl-dir=*)            VXL_DIR="${1#*=}"
                              `VXL_DIR=realpath "${VXL_DIR}"`
                              ;;
      --vtk-dir=*)            VTK_DIR="${1#*=}"
                              `VTK_DIR=realpath "${VTK_DIR}"`
                              ;;
      --itk-dir=*)            ITK_DIR="${1#*=}"
                              `ITK_DIR=realpath "${ITK_DIR}"`
                              ;;
      --clean )               BUILD_CLEAN=1
                              ;;
      --dependencies_only )   BUILD_SHAPEWORKS=0
                              ;;
      -h | --help )           usage
                              exit
                              ;;
      * )                     echo "---------------------"
                              echo "ERROR: Unknown parameter \"$1\""
                              echo "(remember to use '=' between a parameter name and its value)"
                              echo "---------------------"
                              echo "use '--help' to show usage"
                              exit 1
    esac
    shift
  done
}
# test for required version of something (versions with dots are okay):
# usage:
#   at_least_required_version "name" $curr $required
#   if [[ $? -eq 1 ]]; then echo "version sufficient"; fi
at_least_required_version()
{
  packagename=$1
  current=$2
  required=$3
  echo "## verifying ${packagename}..."
  if [ "$(printf '%s\n' "$required" "$current" | sort -V | head -n1)" = "$required" ]; then 
    echo "## SUCCESS: $current is greater than or equal to $required"
    return 1
  else
    echo "## FAILURE: ${packagename} version must be at least $required"
    return 0
  fi
}

build_vxl()
{
  echo "## Building vxl..."


  if [[ $OSTYPE == "msys" ]]; then
      # VXL make install on windows doesn't work, build in INSTALL_DIR
      cd ${INSTALL_DIR}
  else
      cd ${BUILD_DIR}
  fi
  
  git clone https://github.com/vxl/vxl.git
  cd vxl
  # They fixed the VS compilation problem the day after the v2.0.2 release.
  # There hasn't been a release since
  # git checkout -f tags/${VXL_VER}
  git checkout -f c3fd27959f51e0469a7a6075e975f245ac306f3d

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == "msys" ]]; then
      echo cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=Release -Wno-dev ..
      cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=Release -Wno-dev ..
      cmake --build . --config Release || exit 1
  else
      cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_CORE_VIDEO:BOOL=OFF -DBUILD_BRL:BOOL=OFF -DBUILD_CONTRIB:BOOL=OFF -DVNL_CONFIG_LEGACY_METHODS=ON -DVCL_STATIC_CONST_INIT_FLOAT=0 -DVXL_FORCE_V3P_GEOTIFF:BOOL=ON -DVXL_USE_GEOTIFF:BOOL=OFF -DVXL_USE_DCMTK:BOOL=OFF -DCMAKE_BUILD_TYPE=Release -Wno-dev ..
      make -j${NUM_PROCS} install || exit 1
  fi

  VXL_DIR=${INSTALL_DIR}/share/vxl/cmake
}

build_vtk()
{
  echo "## Building vtk..."
  cd ${BUILD_DIR}
  git clone https://gitlab.kitware.com/vtk/vtk.git
  cd vtk
  git checkout -f tags/${VTK_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=${BUILD_GUI} -DVTK_QT_VERSION=5 -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=Release -DVTK_PYTHON_VERSION=3 -Wno-dev ..
      cmake --build . --config Release || exit 1
      cmake --build . --config Release --target install
  else
      cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=${BUILD_GUI} -DVTK_QT_VERSION=5 -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=Release -DVTK_PYTHON_VERSION=3 -Wno-dev ..
      make -j${NUM_PROCS} install || exit 1
  fi

  # TODO: this could be lib (not lib64) on the Windows linux subsystem (or even other linuxes), so need to verify
  if [ "$(uname)" == "Darwin" ]; then
    VTK_LIB_DIR="lib"
  else
    VTK_LIB_DIR="lib64"
  fi    
  VTK_DIR=${INSTALL_DIR}/${VTK_LIB_DIR}/cmake/vtk-${VTK_VER_STR}
}

build_itk()
{
  echo "## Building itk..."
  cd ${BUILD_DIR}
  git clone https://github.com/InsightSoftwareConsortium/ITK.git
  cd ITK
  git checkout -f tags/${ITK_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DVTK_DIR="d:/a/ShapeWorks/deps/lib/cmake/vtk-8.2" -DModule_ITKVtkGlue:BOOL=ON -DCMAKE_BUILD_TYPE=Release -Wno-dev ..
      
      cmake --build . --config Release || exit 1
      cmake --build . --config Release --target install
  else
      cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DModule_ITKVtkGlue:BOOL=ON -DITK_USE_SYSTEM_VXL=on -DVXL_DIR=${INSTALL_DIR} -DCMAKE_BUILD_TYPE=Release -Wno-dev ..
      make -j${NUM_PROCS} install || exit 1
  fi

  ITK_DIR=${INSTALL_DIR}/lib/cmake/ITK-${ITK_VER_STR}
}

build_shapeworks()
{
  echo "## Building ShapeWorks..."
  OPENMP_FLAG="-DUSE_OPENMP=ON"
  if [ "$(uname)" == "Darwin" ]; then
    OPENMP_FLAG="-DUSE_OPENMP=OFF"
  fi

  cd ${BUILD_DIR}
  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf shapeworks-build; fi
  mkdir -p shapeworks-build && cd shapeworks-build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DITK_DIR=${ITK_DIR} -DVXL_DIR=${VXL_DIR} -DVTK_DIR=${VTK_DIR} -DBuild_Post:BOOL=${BUILD_POST} -DBuild_View2:BOOL=${BUILD_GUI} -DBuild_Studio:BOOL=${BUILD_STUDIO} ${OPENMP_FLAG} -Wno-dev -Wno-deprecated -DCMAKE_BUILD_TYPE=Release ${SRC}
  make -j${NUM_PROCS} install || exit 1

  # Inform users of ShapeWorks install path:
  # FIXME/TODO: get rid of LD_LIBRARY_PATH requirement (https://github.com/SCIInstitute/ShapeWorks/issues/125)
  echo "## -----------------------------------------"
  echo "## ShapeWorks has successfully been installed in ${INSTALL_DIR}."
  echo "## Set LD_LIBRARY_PATH for shared libraries to be found:"
  echo "##   export LD_LIBRARY_PATH=${INSTALL_DIR}/lib:${INSTALL_DIR}/lib64:\$LD_LIBRARY_PATH"
  echo "## -----------------------------------------"

}

# determine if we can build using specified or discovered version of Qt
verify_qt()
{
  # If BUILD_GUI is true, verify sufficient version of Qt is installed using the qmake that's in the path.
  # If not, inform the user of where to go to download and install Qt.

  if [[ $BUILD_GUI = 1 ]]; then
    if [[ `which qmake` ]]; then
      QMAKE=`which qmake`
    fi

    # verify version
    if [[ -f ${QMAKE} ]]; then
      QT_VERSION=`${QMAKE} -v | grep "Using Qt version" | cut -d" " -f4`
      at_least_required_version "Qt" "${QT_VERSION}" "${QT_MIN_VER}"
      QT_VERSION_OK=$?
    else
      QT_VERSION_OK=0
    fi

    # inform user
    if [[ $QT_VERSION_OK = 0 ]]; then
      echo '## For GUI applications, please make sure at least version $QT_MIN_VER of Qt5 is installed and that its qmake is in the path.' >&2
      echo '## Download Qt5 from: https://download.qt.io/archive/qt/' >&2
      exit 1
    fi
  fi
}

build_all()
{
  ## create build and install directories
  if [[ -z $BUILD_DIR ]];   then BUILD_DIR=${SRC}/build;     fi
  if [[ -z $INSTALL_DIR ]]; then INSTALL_DIR=${SRC}/install; fi
  mkdir -p ${BUILD_DIR}
  mkdir -p ${INSTALL_DIR}

  ## build dependencies if their locations were not specified
  if [[ -z $VXL_DIR ]]; then
    build_vxl
  fi

  if [[ -z $VTK_DIR ]]; then
    build_vtk
  fi

  if [[ -z $ITK_DIR ]]; then
    build_itk
  fi

  if [[ $BUILD_SHAPEWORKS = 1 ]]; then
      build_shapeworks
  else
    # echo dependency directories for easy reference in case the user is independenly building ShapeWorks
    echo ""
    echo "Dependency paths:"
    echo "  VXL_DIR: ${VXL_DIR}"
    echo "  VTK_DIR: ${VTK_DIR}"
    echo "  ITK_DIR: ${ITK_DIR}"
  fi
}

#
# main
#

SRC=`pwd`
parse_command_line $*

echo "##---------------------"
echo "## ShapeWorks Superbuild"
echo "##---------------------"
echo "##"
echo "## called using these arguments:"
echo "##  $*"
echo "##"
echo ""

verify_qt

echo "INSTALL_DIR: ${INSTALL_DIR}"
echo "BUILD_DIR: ${BUILD_DIR}"
echo "VXL_DIR: ${VXL_DIR}"
echo "VTK_DIR: ${VTK_DIR}"
echo "ITK_DIR: ${ITK_DIR}"
echo "NUM_PROCS: ${NUM_PROCS}"
echo "BUILD_GUI: ${BUILD_GUI}"
echo "BUILD_STUDIO: ${BUILD_STUDIO}"
echo "BUILD_CLEAN: ${BUILD_CLEAN}"

#build ShapeWorks and necessary dependencies
(time build_all 2>&1) 2>&1 | tee ${BUILD_LOG}
