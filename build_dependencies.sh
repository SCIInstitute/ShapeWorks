#!/bin/bash

#set -v   #verbose execution for debugging
#set -x   #tracing execution for debugging (echos all commands from script)

# defaults
BUILD_CLEAN=0
NUM_PROCS=8
BUILD_GUI=1
BUILD_STUDIO=0
BUILD_SHAPEWORKS=1
BUILD_LOG="build_dependencies.log"
VXL_VER="v2.0.2"
VTK_VER="v8.2.0"
VTK_VER_STR="8.2"
ITK_VER="v5.0.1"
ITK_VER_STR="5.0"
EIGEN_VER="3.3.7"
ITK_VER_STR="5.0"
QT_MIN_VER="5.9.8"  # NOTE: 5.x is required, but this restriction is a clever way to ensure the anaconda version of Qt (5.9.6 or 5.9.7) isn't used since it won't work on most systems.
XLNT_VER="v1.4.0"
OpenVDB_VER="v7.0.0"
libigl_VER="v2.2.0"

usage()
{
  echo "usage: ./build_dependencies.sh [[-n=<num-procs>] [-i=<install_path>] [-b=<build_path>] [--clean] [--no-gui] | [-h | --help]]"
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
  echo "  --no-gui                : Do not build the dependencies with GUI support, which require Qt."
  echo "                          : The GUI is built by default if qmake >= $QT_MIN_VER is found in the path."
  echo "  -b,--build-dir=<path>   : Build directory for dependencies."
  echo "                          : By default uses a subdirectory of the current directory called 'dependencies/build'."
  echo "  -i,--install-dir=<path> : Install directory for dependencies."
  echo "                          : By default uses a subdirectory of the current directory called 'dependencies/install'."
  echo "  -n,--num-procs=<num>    : Number of processors to use for parallel builds (default is 8)."
  echo ""
  echo "Example: ./build_dependencies.sh --num-procs=8 --install-dir=/path/to/desired/installation"
  echo "Build results are saved in ${BUILD_LOG}."
}

# full names of relative paths are needed to specify locations of dependencies
function fullpath {
  # unix full paths start with `/`; second character of windows full paths is `:`
  if [[ $1 = /* || ${1:1:1} = \: ]]; then
    echo $1
  else
    echo `pwd`/$1
  fi
}

parse_command_line()
{
  while [ "$1" != "" ]; do
    case $1 in
      -i=*|--install-dir=*)   INSTALL_DIR="${1#*=}"
                              INSTALL_DIR=`fullpath "${INSTALL_DIR}"`
                              ;;
      -b=*|--build-dir=*)     BUILD_DIR="${1#*=}"
                              BUILD_DIR=`fullpath "${BUILD_DIR}"`
                              ;;
      -n=*|--num-procs=*)     NUM_PROCS="${1#*=}"
                              ;;
      --no-gui )              BUILD_GUI=0
                              ;;
      --clean )               BUILD_CLEAN=1
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
  echo ""
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
      echo cmake -DCMAKE_CXX_FLAGS="-FS" -DCMAKE_C_FLAGS="-FS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -Wno-dev ..
      cmake -DCMAKE_CXX_FLAGS="-FS" -DCMAKE_C_FLAGS="-FS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=OFF -DVXL_FORCE_V3P_GEOTIFF:BOOL=ON -DVXL_USE_GEOTIFF:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_CONTRIB:BOOL=OFF -DVNL_CONFIG_LEGACY_METHODS=ON -DVXL_USE_DCMTK:BOOL=OFF -Wno-dev ..
      cmake --build . --config RelWithDebInfo || exit 1
      VXL_DIR=${INSTALL_DIR}/vxl/build
  else
      cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_CORE_VIDEO:BOOL=OFF -DBUILD_BRL:BOOL=OFF -DBUILD_CONTRIB:BOOL=OFF -DVNL_CONFIG_LEGACY_METHODS=ON -DVCL_STATIC_CONST_INIT_FLOAT=0 -DVXL_FORCE_V3P_GEOTIFF:BOOL=ON -DVXL_USE_GEOTIFF:BOOL=OFF -DVXL_USE_DCMTK:BOOL=OFF -DCMAKE_BUILD_TYPE=Release -Wno-dev ..
      make -j${NUM_PROCS} install || exit 1
      VXL_DIR=${INSTALL_DIR}/share/vxl/cmake
  fi
}

build_vtk()
{
  echo ""
  echo "## Building vtk..."
  cd ${BUILD_DIR}
  git clone https://gitlab.kitware.com/vtk/vtk.git
  cd vtk
  git checkout -f tags/${VTK_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=${BUILD_GUI} -DVTK_QT_VERSION=5 -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=RelWithDebInfo -DVTK_PYTHON_VERSION=3 -DVTK_GROUP_ENABLE_Qt=YES -Wno-dev ..
      cmake --build . --config Release || exit 1
      cmake --build . --config Release --target install
      VTK_DIR="${INSTALL_DIR}/lib/cmake/vtk-${VTK_VER_STR}"
      VTK_DIR=$(echo $VTK_DIR | sed s/\\\\/\\//g)
  else
      cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=${BUILD_GUI} -DVTK_QT_VERSION=5 -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=Release -DVTK_PYTHON_VERSION=3 -DVTK_GROUP_ENABLE_Qt=YES -Wno-dev ..
      make -j${NUM_PROCS} install || exit 1
      VTK_DIR=${INSTALL_DIR}/lib/cmake/vtk-${VTK_VER_STR}
  fi
}

build_itk()
{
  echo ""
  echo "## Building itk..."
  cd ${BUILD_DIR}
  git clone https://github.com/InsightSoftwareConsortium/ITK.git
  cd ITK
  git checkout -f tags/${ITK_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_CXX_FLAGS="-FS" -DCMAKE_C_FLAGS="-FS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DVTK_DIR="${VTK_DIR}" -DModule_ITKVtkGlue:BOOL=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -Wno-dev ..
      
      cmake --build . --config RelWithDebInfo || exit 1
      cmake --build . --config RelWithDebInfo --target install
  else
      cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DModule_ITKVtkGlue:BOOL=ON -DITK_USE_SYSTEM_VXL=on -DVXL_DIR=${INSTALL_DIR} -DVTK_DIR=${VTK_DIR} -DCMAKE_BUILD_TYPE=Release -Wno-dev ..
      make -j${NUM_PROCS} install || exit 1
  fi

  ITK_DIR=${INSTALL_DIR}/lib/cmake/ITK-${ITK_VER_STR}
}

build_eigen()
{
  echo ""
  echo "## Building Eigen..."
  cd ${BUILD_DIR}
  git clone https://gitlab.com/libeigen/eigen.git
  cd eigen
  git checkout -f tags/${EIGEN_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_CXX_FLAGS="-FS" -DCMAKE_C_FLAGS="-FS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" ..
      cmake --build . --config RelWithDebInfo || exit 1
      cmake --build . --config RelWithDebInfo --target install
  else
      cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ..
      make -j${NUM_PROCS} install || exit 1
  fi

  EIGEN_DIR=${INSTALL_DIR}/share/eigen3/cmake/
}

build_xlnt()
{
  echo ""
  echo "## Building Xlnt..."
  cd ${BUILD_DIR}
  git clone https://github.com/tfussell/xlnt.git
  cd xlnt
  git checkout -f tags/${XLNT_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  if [[ $OSTYPE == "msys" ]]; then
      cmake -DCMAKE_CXX_FLAGS="-FS" -DCMAKE_C_FLAGS="-FS" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DSTATIC=ON ..
      cmake --build . --config RelWithDebInfo || exit 1
      cmake --build . --config RelWithDebInfo --target install
  else
      cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DSTATIC=ON ..
      make -j${NUM_PROCS} install || exit 1
  fi

  XLNT_DIR=${INSTALL_DIR}
}

build_openvdb()
{
  echo ""
  echo "## Building OpenVDB..."
  cd ${BUILD_DIR}
  git clone https://github.com/AcademySoftwareFoundation/openvdb.git
  cd openvdb
  git checkout ${OpenVDB_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build

  CONCURRENT_FLAG=""
  if [ "$(uname)" == "Darwin" ]; then
      # There is an incompatibility between Qt and tbbmalloc_proxy on Mac
      CONCURRENT_FLAG="-DCONCURRENT_MALLOC=None"
  fi
  
  if [[ $OSTYPE == "msys" ]]; then
      cmake -DUSE_BLOSC=OFF -DCMAKE_PREFIX_PATH=${CONDA_PREFIX} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ..
      cmake --build . --config Release || exit 1
      cmake --build . --config Release --target install
  else
      cmake -DUSE_BLOSC=OFF ${CONCURRENT_FLAG} -DCMAKE_PREFIX_PATH=${CONDA_PREFIX} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib ..
      make -j${NUM_PROCS} install || exit 1
  fi

  OpenVDB_DIR=${INSTALL_DIR}/lib64/cmake/OpenVDB/
}

build_igl()
{
  echo " "
  echo "## Building Libigl..."
  cd ${INSTALL_DIR}
  git clone https://github.com/libigl/libigl.git
  cd libigl
  git checkout -f tags/${libigl_VER}

  LIBIGL_DIR=${INSTALL_DIR}/libigl/cmake
}

show_shapeworks_build()
{
  echo ""
  echo "## To configure ShapeWorks, create a build directory and from there run..."
  echo ""

  OPENMP_FLAG="-DUSE_OPENMP=ON"
  if [ "$(uname)" == "Darwin" ]; then
    OPENMP_FLAG="-DUSE_OPENMP=OFF"
  fi

  FINDQT=""
  if [[ ${BUILD_GUI} -eq 1 ]]; then
    FINDQT="-DQt5_DIR=`qmake -query QT_INSTALL_PREFIX`"
  fi

  echo "cmake -DCMAKE_PREFIX_PATH=${INSTALL_DIR} ${OPENMP_FLAG} -DBuild_Studio=${BUILD_GUI} ${FIND_QT} -Wno-dev -Wno-deprecated -DCMAKE_BUILD_TYPE=Release ${SRC}"
}

# determine if we can build using the specified or discovered version of Qt
verify_qt()
{
  # If BUILD_GUI is true, verify that a sufficient version of Qt is installed using the qmake that's in the path.
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
  if [[ -z $BUILD_DIR ]];   then BUILD_DIR=${SRC}/dependencies/build;     fi
  if [[ -z $INSTALL_DIR ]]; then INSTALL_DIR=${SRC}/dependencies/install; fi
  mkdir -p ${BUILD_DIR}
  mkdir -p ${INSTALL_DIR}

  ## build dependencies if their locations were not specified
  if [[ -z $OpenVDB_DIR ]]; then
    build_openvdb
  fi

  if [[ -z $VXL_DIR ]]; then
    build_vxl
  fi

  if [[ -z $VTK_DIR ]]; then
    build_vtk
  fi

  if [[ -z $ITK_DIR ]]; then
    build_itk
  fi

  if [[ -z $EIGEN_DIR ]]; then
    build_eigen
  fi

  if [[ -z $XLNT_DIR ]]; then
    build_xlnt
  fi

  if [[ -z $LIBIGL_DIR ]]; then
    build_igl
  fi

  # echo dependency directories for easy reference in case the user is independently building ShapeWorks
  echo ""
  echo "Dependency paths:"
  echo "  VXL_DIR: ${VXL_DIR}"
  echo "  VTK_DIR: ${VTK_DIR}"
  echo "  ITK_DIR: ${ITK_DIR}"
  echo "  EIGEN_DIR: ${EIGEN_DIR}"
  echo "  OpenVDB_DIR: ${OpenVDB_DIR}"
  echo "  LIBIGL_DIR: ${LIBIGL_DIR}"
  echo ""
  
  show_shapeworks_build
}

#
# main
#

SRC=`pwd`
parse_command_line $*

echo "##-------------------------------"
echo "## ShapeWorks Build Dependencies"
echo "##-------------------------------"
echo "##"
echo "## called using these arguments:"
echo "##  $*"
echo "##"
echo ""

if [[ $OSTYPE != "msys" ]]; then
    verify_qt
fi

echo "INSTALL_DIR: ${INSTALL_DIR}"
echo "BUILD_DIR: ${BUILD_DIR}"
echo "NUM_PROCS: ${NUM_PROCS}"
echo "BUILD_GUI: ${BUILD_GUI}"
echo "BUILD_CLEAN: ${BUILD_CLEAN}"

#build dependencies
(time build_all 2>&1) 2>&1 | tee ${BUILD_LOG}

