echo "ShapeWorks superbuild for Mac"

# Call this script by specifying arguments in the same command.
# Ex:
#   BUILD_VIEW2=0 NUM_PROCS=16 ./superbuild-linux.sh
#

# helper functions
. ./superbuild-utils.sh

# force rebuild of everything
if [ -z $BUILD_CLEAN ]; then
  BUILD_CLEAN=0
fi

# Post is built by default
if [ -z $BUILD_POST ]; then
  BUILD_POST=1
fi

# vtk will be built with QT only if this is ON
if [ -z $BUILD_VIEW2 ]; then
  BUILD_VIEW2=1
fi

if [ $BUILD_VIEW2 != 0 ]; then
  if ! [ -x "$(command -v qmake)" ]; then
    echo 'Error: Please make sure Qt4 is installed and that qmake is in the path.' >&2
    echo 'Please Download Qt4 from: https://download.qt.io/archive/qt/4.8/4.8.7/' >&2
    exit 1
  fi
fi

# builds faster if more cores are available
if [ -z $NUM_PROCS ]; then
  NUM_PROCS=4
fi

# Anaconda is highly recommended in order to build multiple versions of various software. See INSTALL.md for details.
if [ -z $USING_CONDA_SANDBOX ]; then
  USING_CONDA_SANDBOX=0
fi

## which dependencies should be built ##
if [ -z $Build_VXL ]; then
  Build_VXL=1
fi
if [ -z $Build_VTK ]; then
  Build_VTK=1
fi
if [ -z $Build_ITK ]; then
  Build_ITK=1
fi

## create build and install directories for ShapeWorks deps ##
if [ $USING_CONDA_SANDBOX ]; then
  root=${CONDA_PREFIX}
  INSTALL_PREFIX="${CONDA_PREFIX}"
else
  root=$(pwd)
  INSTALL_PREFIX="${depDir}/install"
fi
depDir=${root}/shapeworks-dependencies
if [ $BUILD_CLEAN != 0 ]; then rm -rf ${depDir}; fi
mkdir ${depDir}
mkdir ${depDir}/install

######## VXL ########
if [ $Build_VXL -eq 1 ]
then
  VXL_VER="github-migration"
  VXL_BUILD_DIR=${depDir}/vxl-build-${VXL_VER}
  cd ${depDir}
  git clone https://github.com/vxl/vxl.git
  cd vxl/
  git checkout -f tags/${VXL_VER}
  sed -i '1,10 s/^/#/' CMakeLists.txt
  sed -i 's/JPEG_FOUND/JPEG_FOUND_DISABLE/' core/vil/CMakeLists.txt
  sed -i 's/JPEG_FOUND/JPEG_FOUND_DISABLE/' core/vil1/CMakeLists.txt
  cd ${depDir}

  if [ $BUILD_CLEAN != 0 ]; then rm -rf ${VXL_BUILD_DIR}; fi
  mkdir ${VXL_BUILD_DIR}
  cd ${VXL_BUILD_DIR}
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_CORE_VIDEO:BOOL=OFF -DBUILD_BRL:BOOL=OFF -DBUILD_CONTRIB:BOOL=OFF -DVNL_CONFIG_LEGACY_METHODS=ON -DVCL_STATIC_CONST_INIT_FLOAT=0 -Wno-dev ../vxl/
  make -j{NUM_PROCS} install
fi

######## VTK ########
if [ $Build_VTK -eq 1 ]
then
  VTK_VER="v5.10.1"
  VTK_BUILD_DIR=${depDir}/vtk-build-${VTK_VER}
  cd ${depDir}
  git clone https://gitlab.kitware.com/vtk/vtk.git
  cd vtk/
  git checkout -f tags/${VTK_VER}
  cd ${depDir}

  if [ $BUILD_CLEAN != 0 ]; then rm -rf ${VTK_BUILD_DIR}; fi
  mkdir ${VTK_BUILD_DIR}
  cd ${VTK_BUILD_DIR}

  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DVTK_USE_QT:BOOL=${BUILD_VIEW2} -DCMAKE_C_FLAGS=-DGLX_GLXEXT_LEGACY -DCMAKE_CXX_FLAGS=-DGLX_GLXEXT_LEGACY -DVTK_USE_SYSTEM_TIFF=ON -DVTK_REQUIRED_OBJCXX_FLAGS="" -Wno-dev ../vtk/
  make -j{NUM_PROCS} install
fi

######## ITK ########
if [ $Build_ITK -eq 1 ]
then
  ITK_VER="v4.7.2"
  ITK_BUILD_DIR=${depDir}/itk-build-${ITK_VER}
  cd ${depDir}
  git clone https://github.com/InsightSoftwareConsortium/ITK.git
  /usr/bin/sed -i bak 's/nout > 0/this->nout/' Modules/ThirdParty/VNL/src/vxl/v3p/netlib/linalg/lsqrBase.cxx
  cd ITK/
  git checkout -f tags/${ITK_VER}
  cd ${depDir}

  if [ $BUILD_CLEAN != 0 ]; then rm -rf ${ITK_BUILD_DIR}; fi
  mkdir ${ITK_BUILD_DIR}
  cd ${ITK_BUILD_DIR}
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DITK_USE_SYSTEM_VXL=on -Wno-dev ../ITK/
  make -j{NUM_PROCS} install
fi

######## ShapeWorks ########
cd ${root}
if [ $BUILD_CLEAN != 0 ]; then rm -rf shapeworks-build; fi
mkdir shapeworks-build/
cd shapeworks-build/

if ! [ $USING_CONDA_SANDBOX ]; then
  ITK_DIR="-DITK_DIR=${ITK_BUILD_DIR}"
  VTK_DIR="-DVTK_DIR=${VTK_BUILD_DIR}"
  VXL_DIR="-DVXL_DIR=${VXL_BUILD_DIR}"
fi

cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} ${ITK_DIR} ${VXL_DIR} ${VTK_DIR} -DBuild_Post:BOOL=${BUILD_POST} -DBuild_View2:BOOL=${BUILD_VIEW2} -DUSE_OPENMP=OFF -Wno-dev -Wno-deprecated ../shapeworks
make -j{NUM_PROCS} install
cd ${root}

