#!/bin/bash

#set -v   #verbose execution for debugging

# defaults
BUILD_CLEAN=0
NUM_PROCS=8
BUILD_GUI=1
BUILD_STUDIO=0
BUILD_LOG="shapeworks_superbuild.log"
VXL_VER="v2.0.2"
VTK_VER="v8.2.0"
ITK_VER="v5.0.1"
QT_DIR=""

usage()
{
  echo "ShapeWorks superbuild"
  echo "---------------------"
  echo "usage: ./superbuild.sh [[-n=<num-procs>] [-i=<install_path>] [-b=<build_path>] [--clean] [--no-gui] [--qt-dir=<qt_path>] [--vxl-dir=<vxl_path>] [--vtk-dir=<vtk_path>] [--itk-dir=<itk_path>] | [-h | --help]]"
  echo ""
  echo "If using Anaconda to install prerequisites please first run:"
  echo "source ./conda_installs.sh"
  echo ""
  echo "Arguments:"
  echo "  -h,--help               : Show this screen"
  echo "  --clean                 : Remove all build directories and clone implicit dependencies"
  echo "                          : (explicitly specified dependenciea such as --itk-dir=<path> are ignored)"
  echo "  --no-gui                : Do not build the ShapeWorks gui applicaitons, which require Qt 5.x"
  echo "                          : The GUI is built by default if qmake > 5.x is found in the path or user-specified QT_DIR."
  echo "  --with-studio           : Build ShapeWorksStudio (default off)"
  echo "  -b,--build-dir=<path>   : Build directory for ShapeWorks and its implicit dependencieas (VXL, VTK, and ITK)"
  echo "                          : By default uses a subdirectory of the current directory called 'build'."
  echo "  -i,--install-dir=<path> : Install directory for ShapeWorks and its implicit dependencieas (VXL, VTK, and ITK)"
  echo "                          : By default uses a subdirectory of the current directory called 'install'."
  echo "  -n,--num_procs=<num>    : Number of processors to use for parallel builds (default is 8)"
  echo "  --vxl-dir=<path>        : Path to existing VXL installation (version >= ${VXL_VER})"
  echo "  --vtk-dir=<path>        : Path to existing VTK installation (version >= ${VTK_VER})"
  echo "  --itk-dir=<path>        : Path to existing ITK installation (version >= ${ITK_VER})"
  echo "  --qt-dir=<path>         : The path to Qt version 5.x that is installed on your system"
  echo ""
  echo "Example: ./superbuild.sh -n 8 --qt-dir /path/to/qt"
  echo "Build results are saved in ${BUILD_LOG}."
}

parse_command_line()
{
  while [ "$1" != "" ]; do
    case $1 in
      -i=*|--install-dir=*)   INSTALL_DIR="${1#*=}"
                              ;;
      -b=*|--build-dir=*)     BUILD_DIR="${1#*=}"
                              ;;
      -n=*|--num-procs=*)     NUM_PROCS="${1#*=}"
                              ;;
      --no-gui )              BUILD_GUI=0
                              ;;
      --with-studio )         BUILD_STUDIO=1
                              ;;
      --qt-dir=*)             QT_DIR="${1#*=}"
                              ;;
      --vxl-dir=*)            VXL_DIR="${1#*=}"
                              ;;
      --vtk-dir=*)            VTK_DIR="${1#*=}"
                              ;;
      --itk-dir=*)            ITK_DIR="${1#*=}"
                              ;;
      --clean )               BUILD_CLEAN=1
                              ;;
      -h | --help )           usage
                              exit
                              ;;
      * )                     usage
                              exit 1
    esac
    shift
  done
}

build_vxl()
{
  echo "## Building vxl..."
  VXL_DIR=${BUILD_DIR}/vxl
  cd ${BUILD_DIR}
  git clone https://github.com/vxl/vxl.git
  cd ${VXL_DIR}
  git checkout -f tags/${VXL_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_CORE_VIDEO:BOOL=OFF -DBUILD_BRL:BOOL=OFF -DBUILD_CONTRIB:BOOL=OFF -DVNL_CONFIG_LEGACY_METHODS=ON -DVCL_STATIC_CONST_INIT_FLOAT=0 -DVXL_FORCE_V3P_GEOTIFF:BOOL=ON -DVXL_USE_GEOTIFF:BOOL=OFF -DVXL_USE_DCMTK:BOOL=OFF -DCMAKE_BUILD_TYPE=Release -Wno-dev ${VXL_DIR}
  make -j${NUM_PROCS} install || exit 1
}

build_vtk()
{
  echo "## Building vtk..."
  VTK_DIR=${BUILD_DIR}/vtk
  cd ${BUILD_DIR}
  git clone https://gitlab.kitware.com/vtk/vtk.git
  cd ${VTK_DIR}
  git checkout -f tags/${VTK_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=${BUILD_GUI} -DVTK_QT_VERSION=5 -DCMAKE_BUILD_TYPE=Release -DVTK_PYTHON_VERSION=3 -Wno-dev ${VTK_DIR}
  make -j${NUM_PROCS} install || exit 1
}

build_itk()
{
  echo "## Building itk..."
  ITK_DIR=${BUILD_DIR}/ITK
  cd ${BUILD_DIR}
  git clone https://github.com/InsightSoftwareConsortium/ITK.git
  cd ${ITK_DIR}
  git checkout -f tags/${ITK_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DModule_ITKVtkGlue:BOOL=ON -DITK_USE_SYSTEM_VXL=on -DVXL_DIR=${INSTALL_DIR} -DCMAKE_BUILD_TYPE=Release -Wno-dev ${ITK_DIR}
  make -j${NUM_PROCS} install || exit 1
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
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DITK_DIR=${INSTALL_DIR}/lib/cmake/ITK-5.0 -DVXL_DIR=${INSTALL_DIR}/share/vxl/cmake -DVTK_DIR=${INSTALL_DIR}/lib64/cmake/vtk-8.2 -DBuild_Post:BOOL=${BUILD_POST} -DBuild_View2:BOOL=${BUILD_GUI} -DBuild_Studio:BOOL=${BUILD_STUDIO} ${OPENMP_FLAG} -Wno-dev -Wno-deprecated -DCMAKE_BUILD_TYPE=Release ${SRC}
  make -j${NUM_PROCS} install || exit 1

  # Inform users of ShapeWorks install path:
  echo "## -----------------------------------------"
  echo "## ShapeWorks has successfully been installed in ${INSTALL_DIR}."
  echo "## Set LD_LIBRARY_PATH for shared libraries to be found:"
  echo "##   export LD_LIBRARY_PATH=${INSTALL_DIR}/lib:${INSTALL_DIR}/lib64:\$LD_LIBRARY_PATH"
  echo "## -----------------------------------------"

}

# determine if we can build using specified or discovered version of Qt
find_qt()
{
  # Check for qmake in the specified directory first, in the path second, and
  # if not found (or of insufficient version) and BUILD_GUI is true, inform
  # the user of where to go to download and install Qt.

  if [[ BUILD_GUI = 1 ]]; then
    # set QMAKE
    if [[ ! -z "${QT_DIR}" ]]; then
      QMAKE=${QT_DIR}/bin/qmake
      if [[ ! -f ${QMAKE} ]]; then
        echo "ERROR: Qt not found (incorrect path specified)"
        exit
      fi
    elif [[ `which qmake` ]]; then
      QMAKE=`which qmake`
    fi

    # verify version
    if [[ -f ${QMAKE} ]]; then
      QT_VERSION=`${QT_DIR}/bin/qmake -v | grep "Using Qt version" | cut -d" " -f4`
      at_least_required_version "Qt" $QT_VERSION 5.0
      QT_VERSION_OK=$?
    else
      QT_VERSION_OK=0
    fi

    # inform user
    if [[ $QT_VERSION_OK = 0 ]]; then
      echo '## For GUI applications, please make sure Qt5 is installed and that eithe qmake is in the path or the --qt-dir is specifed.' >&2
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

  build_shapeworks
}

#
# main
#

echo "## ShapeWorks Superbuild"

# helper functions
SRC=`pwd`
. ${SRC}/superbuild-utils.sh

parse_command_line $*
find_qt

echo "INSTALL_DIR: ${INSTALL_DIR}"
echo "BUILD_DIR: ${BUILD_DIR}"
echo "QT_DIR: ${QT_DIR}"
echo "VXL_DIR: ${VXL_DIR}"
echo "VTK_DIR: ${VTK_DIR}"
echo "ITK_DIR: ${ITK_DIR}"
echo "NUM_PROCS: ${NUM_PROCS}"
echo "BUILD_GUI: ${BUILD_GUI}"
echo "BUILD_STUDIO: ${BUILD_STUDIO}"
echo "BUILD_CLEAN: ${BUILD_CLEAN}"

#build ShapeWorks and necessary dependencies
(time build_all 2>&1) 2>&1 | tee shapeworks_superbuild.log

