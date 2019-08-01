#!/bin/bash

echo "ShapeWorks superbuild for Linux"

# Call this script by specifying arguments in the same command.
# Ex:
#   NUM_PROCS=16 ./superbuild-linux.sh
#
# Arguments:
#  BUILD_CLEAN:   whether or not to remove all build directories and clone new dependencies
#
#  VXL_DIR:       if you already have VXL its install path can be specified
#  VTK_DIR:       if you already have VTK its install path can be specified
#  ITK_DIR:       if you already have ITK its install path can be specified
#
#  BUILD_DIR:     by default creates a subdirectory called build in the current directory
#  INSTALL_DIR:   by default installs ShapeWorks and all external dependencies in a subdirectory called install
#  NUM_PROCS:     number of processors to use for parallel builds (default is 4)
#
#  BUILD_POST:    whether or not to build any applications in Post (default is to build them all)
# HAVE_QT: weather or not qt version 4.x is installed in your system, set 0 if not and it will skip building shapeworksview2
#


# helper functions
SRC=$(dirname "$(readlink -f "$0")")
. ${SRC}/superbuild-utils.sh

# force rebuild of everything
if [ -z $BUILD_CLEAN ]; then
  BUILD_CLEAN=0
fi

if [ -z $HAVE_QT ]; then
  HAVE_QT=0
fi
#
# gcc5 and above required for Post
# if [ at_least_required_version "gcc" `gcc -dumpversion` "5.0.0" -eq 1 ]; then
#   BUILD_POST=1
# else
#   echo "Cannot build Post because gcc version must be at least 5.0.0"
#   BUILD_POST=0
# fi
#
# TODO: 1) Look into why 5.0 is required and maybe fix it since most of our servers only have gcc 4.8.5
#          Also note that Post builds fine on SuSE 42.3 with gcc 4.8.5, so we'll go ahead and build it by default for now.
#          It may not work in Red Hat Linux, mentioned below as a problem when compiling the old version of VTK.
if [ -z $BUILD_POST ]; then
  BUILD_POST=1
fi

# Look for Qt in order to build GUI applications
if [ $HAVE_QT == 1 ]; then
  echo 'For GUI applications, please make sure Qt4 is installed and that qmake is in the path.' >&2
  echo 'Download Qt4 from: https://download.qt.io/archive/qt/4.8/4.8.7/' >&2
fi

#echo $HAVE_QT

# builds faster if more cores are available
if [ -z $NUM_PROCS ]; then
  NUM_PROCS=4
fi

## create build and install directories
if [ -z $BUILD_DIR ]; then
  BUILD_DIR=$(pwd)/build
fi
if [ -z $INSTALL_DIR ]; then
  INSTALL_DIR=$(pwd)/install
fi
mkdir ${BUILD_DIR}
mkdir ${INSTALL_DIR}

## build dependencies if their locations were not specified
if [ -z $VXL_DIR ]; then
  VXL_VER="github-migration"
  VXL_DIR=${BUILD_DIR}/vxl
  cd ${BUILD_DIR}
  git clone https://github.com/vxl/vxl.git
  cd ${VXL_DIR}
  git checkout -f tags/${VXL_VER}
  sed -i '1,10 s/^/#/' CMakeLists.txt
  sed -i 's/JPEG_FOUND/JPEG_FOUND_DISABLE/' core/vil/CMakeLists.txt
  sed -i 's/JPEG_FOUND/JPEG_FOUND_DISABLE/' core/vil1/CMakeLists.txt
  
  if [ $BUILD_CLEAN != 0 ]; then rm -rf build; fi
  mkdir build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_CORE_VIDEO:BOOL=OFF -DBUILD_BRL:BOOL=OFF -DBUILD_CONTRIB:BOOL=OFF -DVNL_CONFIG_LEGACY_METHODS=ON -DVCL_STATIC_CONST_INIT_FLOAT=0 -Wno-dev ${VXL_DIR}
  make -j${NUM_PROCS} install
fi

if [ -z $VTK_DIR ]; then
  VTK_VER="v5.10.1"
  VTK_DIR=${BUILD_DIR}/vtk
  cd ${BUILD_DIR}
  git clone https://gitlab.kitware.com/vtk/vtk.git
  cd ${VTK_DIR}
  git checkout -f tags/${VTK_VER}

  ## fix for deprecated sstream in vtkIOStream.h, needed for gcc 4.8.5 in Red Hat Linux
  sed -i "108,112 s/^/\/\//" vtk/Common/vtkIOStream.h
  sed -i "113i #include <sstream>" vtk/Common/vtkIOStream.h
  sed -i "114i using std::basic_istringstream;" vtk/Common/vtkIOStream.h
  sed -i "115i using std::basic_ostringstream;" vtk/Common/vtkIOStream.h
  sed -i "116i using std::basic_stringstream;" vtk/Common/vtkIOStream.h

  if [ $BUILD_CLEAN != 0 ]; then rm -rf build; fi
  mkdir build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_USE_QT:BOOL=${HAVE_QT} -DCMAKE_C_FLAGS=-DGLX_GLXEXT_LEGACY -DCMAKE_CXX_FLAGS=-DGLX_GLXEXT_LEGACY -Wno-dev ${VTK_DIR}
  make -j${NUM_PROCS} install
fi

if [ -z $ITK_DIR ]; then
  ITK_VER="v4.7.2"
  ITK_DIR=${BUILD_DIR}/ITK
  cd ${BUILD_DIR}
  git clone https://github.com/InsightSoftwareConsortium/ITK.git
  cd ${ITK_DIR}
  git checkout -f tags/${ITK_VER}

  if [ $BUILD_CLEAN != 0 ]; then rm -rf build; fi
  mkdir build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DITK_USE_SYSTEM_VXL=on -Wno-dev ${ITK_DIR}
  make -j${NUM_PROCS} install
fi

## Build ShapeWorks
cd ${BUILD_DIR}
if [ $BUILD_CLEAN != 0 ]; then rm -rf shapeworks-build; fi
mkdir shapeworks-build/
cd shapeworks-build/

cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DITK_DIR=${INSTALL_DIR} -DVXL_DIR=${INSTALL_DIR} -DVTK_DIR=${INSTALL_DIR} -DBuild_Post:BOOL=${BUILD_POST} -DBuild_View2:BOOL=${HAVE_QT} -Wno-dev -Wno-deprecated ${SRC}
make -j${NUM_PROCS} install

# Inform users of ShapeWorks install path:
echo "-----------------------------------------"
echo "ShapeWorks has been installed in ${INSTALL_DIR}."
echo "Set LD_LIBRARY_PATH for shared libraries to be found:"
echo "  export LD_LIBRARY_PATH=`pwd`/install/lib:`pwd`/install/lib/vtk-5.10$LD_LIBRARY_PATH"
echo "-----------------------------------------"
