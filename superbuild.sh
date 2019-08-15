#!/bin/bash

superbuild()
{

echo "ShapeWorks Superbuild"

# Call this script by specifying arguments in the same command.
# Ex:
#   NUM_PROCS=16 HAVE_QT=1 ./superbuild.sh
#
# Arguments:
#
#  BUILD_CLEAN:   whether or not to remove all build directories and clone new dependencies
#
#  VXL_DIR:       if you already have VXL its install path can be specified
#  VTK_DIR:       if you already have VTK its install path can be specified
#  ITK_DIR:       if you already have ITK its install path can be specified
#
#  BUILD_DIR:     by default creates a subdirectory of the current directory called 'build' where ShapeWorks and all its external dependencies will be built
#  INSTALL_DIR:   by default creates a subdirectory of the current directory called 'install' where ShapeWorks and all external dependencies are installed
#  NUM_PROCS:     number of processors to use for parallel builds (default is 4)
#
#  BUILD_POST:    whether or not to build any applications in Post (default is to build them all, see https://github.com/SCIInstitute/ShapeWorks/issues/58 to understand why this is even an option here)
#  HAVE_QT:       whether or not qt version 4.x is installed in your system, set 0 if not and it will skip building shapeworksview2
#

# helper functions
SRC=`pwd`
. ${SRC}/superbuild-utils.sh

if [[ -z $BUILD_CLEAN ]]; then BUILD_CLEAN=0; fi
if [[ -z $HAVE_QT ]];     then HAVE_QT=0;     fi
if [[ -z $BUILD_POST ]];  then BUILD_POST=1;  fi
if [[ -z $NUM_PROCS ]];   then NUM_PROCS=4;   fi
if [[ $HAVE_QT = 1 ]]; then
  echo 'For GUI applications, please make sure Qt5 is installed and that qmake is in the path.' >&2
  echo 'Download Qt5 from: https://download.qt.io/archive/qt/' >&2
fi

## create build and install directories
if [[ -z $BUILD_DIR ]];   then BUILD_DIR=${SRC}/build;     fi
if [[ -z $INSTALL_DIR ]]; then INSTALL_DIR=${SRC}/install; fi
mkdir -p ${BUILD_DIR}
mkdir -p ${INSTALL_DIR}

## build dependencies if their locations were not specified
if [[ -z $VXL_DIR ]]; then
  VXL_VER="v2.0.2"
  VXL_DIR=${BUILD_DIR}/vxl
  cd ${BUILD_DIR}
  git clone https://github.com/vxl/vxl.git
  cd ${VXL_DIR}
  git checkout -f tags/${VXL_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_CORE_VIDEO:BOOL=OFF -DBUILD_BRL:BOOL=OFF -DBUILD_CONTRIB:BOOL=OFF -DVNL_CONFIG_LEGACY_METHODS=ON -DVCL_STATIC_CONST_INIT_FLOAT=0 -DVXL_FORCE_V3P_GEOTIFF:BOOL=ON -DVXL_USE_GEOTIFF:BOOL=OFF -DVXL_USE_DCMTK:BOOL=OFF -DCMAKE_BUILD_TYPE=Release -Wno-dev ${VXL_DIR}
  make -j${NUM_PROCS} install || exit 1
fi

if [[ -z $VTK_DIR ]]; then
  VTK_VER="v8.2.0"
  VTK_DIR=${BUILD_DIR}/vtk
  cd ${BUILD_DIR}
  git clone https://gitlab.kitware.com/vtk/vtk.git
  cd ${VTK_DIR}
  git checkout -f tags/${VTK_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=${HAVE_QT} -DVTK_QT_VERSION=5 -DCMAKE_BUILD_TYPE=Release -DVTK_PYTHON_VERSION=3 -Wno-dev ${VTK_DIR}
  make -j${NUM_PROCS} install || exit 1
fi

if [[ -z $ITK_DIR ]]; then
  ITK_VER="v5.0.1"
  ITK_DIR=${BUILD_DIR}/ITK
  cd ${BUILD_DIR}
  git clone https://github.com/InsightSoftwareConsortium/ITK.git
  cd ${ITK_DIR}
  git checkout -f tags/${ITK_VER}

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DITK_USE_SYSTEM_VXL=on -DVXL_DIR=${INSTALL_DIR} -DCMAKE_BUILD_TYPE=Release -Wno-dev ${ITK_DIR}
  make -j${NUM_PROCS} install || exit 1
fi

OPENMP_FLAG="-DUSE_OPENMP=ON"
if [ "$(uname)" == "Darwin" ]; then
    OPENMP_FLAG="-DUSE_OPENMP=OFF"
fi

## Build ShapeWorks
cd ${BUILD_DIR}
if [[ $BUILD_CLEAN = 1 ]]; then rm -rf shapeworks-build; fi
mkdir -p shapeworks-build && cd shapeworks-build
cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DITK_DIR=${INSTALL_DIR} -DVXL_DIR=${INSTALL_DIR} -DVTK_DIR=${INSTALL_DIR} -DBuild_Post:BOOL=${BUILD_POST} -DBuild_View2:BOOL=${HAVE_QT} ${OPENMP_FLAG} -Wno-dev -Wno-deprecated -DCMAKE_BUILD_TYPE=Release ${SRC}
make -j${NUM_PROCS} install || exit 1

# Inform users of ShapeWorks install path:
echo "-----------------------------------------"
echo "ShapeWorks has successfully been installed in ${INSTALL_DIR}."
echo "Set LD_LIBRARY_PATH for shared libraries to be found:"
echo "  export LD_LIBRARY_PATH=${INSTALL_DIR}/lib:\$LD_LIBRARY_PATH"
echo "-----------------------------------------"

}

(time superbuild 2>&1) 2>&1 | tee shapeworks_superbuild.log
