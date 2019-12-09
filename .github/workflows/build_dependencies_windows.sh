#!/bin/bash -x

INSTALL_DIR=$1

INSTALL_MSYS_DIR="/d/a/ShapeWorks/deps"

BUILD_DIR=/d/a/bdeps
mkdir $BUILD_DIR
mkdir $INSTALL_MSYS_DIR

cd /d/a/

#set -v   #verbose execution for debugging

# defaults
BUILD_CLEAN=0
NUM_PROCS=8
BUILD_GUI=1
BUILD_STUDIO=1
BUILD_LOG="shapeworks_superbuild.log"
VXL_VER="v2.0.2"
VTK_VER="v8.2.0"
VTK_VER_STR="8.2"
ITK_VER="v5.0.1"
ITK_VER_STR="5.0"
QT_MIN_VER="5.9.8"  # NOTE: 5.x is required, but this restriction is a clever way to ensure the anaconda version of Qt (5.9.6 or 5.9.7) isn't used since it won't work on most systems.


build_vxl()
{
  echo "## Building vxl..."
  # cannot use $INSTALL_DIR here as it's a windows style path
  cd $INSTALL_MSYS_DIR
  git clone https://github.com/vxl/vxl.git
  cd vxl
  # They fixed the VS compilation problem the day after the v2.0.2 release.
  # There hasn't been a release since
  git checkout -f c3fd27959f51e0469a7a6075e975f245ac306f3d

  if [[ $BUILD_CLEAN = 1 ]]; then rm -rf build; fi
  mkdir -p build && cd build
#  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_CORE_VIDEO:BOOL=OFF -DBUILD_BRL:BOOL=OFF -DBUILD_CONTRIB:BOOL=OFF -DVNL_CONFIG_LEGACY_METHODS=ON -DVCL_STATIC_CONST_INIT_FLOAT=0 -DVXL_FORCE_V3P_GEOTIFF:BOOL=ON -DVXL_USE_GEOTIFF:BOOL=OFF -DVXL_USE_DCMTK:BOOL=OFF -DCMAKE_BUILD_TYPE=Release -Wno-dev ..
  cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=Release -Wno-dev ..
  cmake --build . --config Release || exit 1
#  cmake --build . --config Release --target install

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
  cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_Group_Qt:BOOL=${BUILD_GUI} -DVTK_QT_VERSION=5 -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=Release -DVTK_PYTHON_VERSION=3 -Wno-dev ..
  cmake --build . --config Release || exit 1
  cmake --build . --config Release --target install

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
#  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DModule_ITKVtkGlue:BOOL=ON -DITK_USE_SYSTEM_VXL=on -DVXL_DIR=${INSTALL_DIR} -DCMAKE_BUILD_TYPE=Release -Wno-dev ..

  cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DVTK_DIR="${INSTALL_DIR}/lib/cmake/vtk-8.2" -DModule_ITKVtkGlue:BOOL=ON -DCMAKE_BUILD_TYPE=Release -Wno-dev ..

  cmake --build . --config Release || exit 1
  cmake --build . --config Release --target install

  ITK_DIR=${INSTALL_DIR}/lib/cmake/ITK-${ITK_VER_STR}
}


build_all()
{
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

#  build_shapeworks
}

#
# main
#

echo "## superbuids.sh $*"
echo "##"
echo "## ShapeWorks Superbuild"
echo "##---------------------"

SRC=`pwd`

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
(time build_all 2>&1) 2>&1 | tee shapeworks_superbuild.log
